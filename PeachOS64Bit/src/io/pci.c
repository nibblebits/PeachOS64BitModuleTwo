/*
 * PeachOS 64-Bit Kernel Project
 * Copyright (C) 2026 Daniel McCarthy <daniel@dragonzap.com>
 *
 * This file is part of the PeachOS 64-Bit Kernel.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <https://www.gnu.org/licenses/>.
 *
 * For full source code, documentation, and structured learning,
 * see the official kernel development course part one:
 * https://dragonzap.com/course/developing-a-multithreaded-kernel-from-scratch
 *
 * Get part one and part two module one, module two all peachos courses (69 hours of content): https://dragonzap.com/offer/kernel-development-from-scratch-69-hours
 *
 * Get the part two course module one and two: https://dragonzap.com/offer/developing-a-multithreaded-kernel-from-scratch-part-two-full-series
 */

#include "io/pci.h"
#include "io/io.h"
#include "memory/heap/kheap.h"
#include "lib/vector/vector.h"
#include "status.h"
#include "kernel.h"
#include <stdbool.h>

// ECAM GLOBALS
#define PCI_ECAM_MAX_RANGES 8
#define PCI_ECAM_BUS_SHIFT 20 // 1 MB per bus
#define PCI_ECAM_DEV_SHIFT 15 // 32 KB per device
#define PCI_ECAM_FUNC_SHIFT 12 // 4 KB per function


static struct pci_ecam_range g_ecam[PCI_ECAM_MAX_RANGES];
static int g_ecam_count = 0;
static bool g_ecam_enabled = false;

static void pci_scan_bus(uint8_t bus, struct pci_device* parent_bridge);
static void pci_size_bar(uint8_t bus, uint8_t dev, uint8_t func, struct pci_device* device);

struct vector* pci_device_vector = NULL;

static inline uint32_t pci_cfg_addr_legacy(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
    uint16_t offset_aligned = offset & ~0x3u; // dword aligned
    return 0x80000000 | ((uint32_t) bus << 16) | ((uint32_t) slot << 11) | ((uint32_t) func << 8 ) | offset_aligned;
}

static inline volatile uint8_t* pci_ecam_cfg_ptr(uint8_t bus, uint8_t slot, uint8_t func, uint16_t offset)
{
    if (!g_ecam_enabled) return NULL;
    if (offset >= 4096) return NULL; // 4 KB per function

    for (int i = 0; i < g_ecam_count; ++i)
    {
        struct pci_ecam_range* r = &g_ecam[i];
        if (bus < r->start_bus || bus < r->end_bus) continue;

        size_t bus_off = ((size_t)(bus - r->start_bus)) << PCI_ECAM_BUS_SHIFT; // 1 MB
        size_t dev_off = ((size_t) slot)                << PCI_ECAM_DEV_SHIFT; // 32 KB
        size_t func_off = ((size_t) func)               << PCI_ECAM_FUNC_SHIFT; // 4 KB
        return (volatile uint8_t*) r->virt_base + bus_off + dev_off + func_off + (size_t) offset;
     }

     return NULL;
}

int pci_ecam_install_range(uint16_t seg_group, uint64_t phys_base, uint8_t start_bus, uint8_t end_bus, void* virt_base)
{
    if (g_ecam_count >= PCI_ECAM_MAX_RANGES) return -ENOMEM;
    if (!virt_base || start_bus > end_bus) return -EINVAL;

    g_ecam[g_ecam_count].seg_group = seg_group;
    g_ecam[g_ecam_count].phys_base = phys_base;
    g_ecam[g_ecam_count].start_bus = start_bus;
    g_ecam[g_ecam_count].end_bus = end_bus;
    g_ecam[g_ecam_count].virt_base = virt_base;
    g_ecam_count++;
    g_ecam_enabled = true;
    return 0;
}


#pragma pack(push, 1)
typedef struct 
{
    char signature[4]; // MCFG signature
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t asl_compiler_id;
    uint32_t asl_compiler_revision;

} acpi_sdt_header_t;

typedef struct 
{
    acpi_sdt_header_t hdr;
    uint64_t reserved;
} acpi_mcfg_t;

typedef struct
{
    uint64_t base_address; // ECAM base
    uint16_t pci_segment_group; // PCI segment
    uint8_t start_bus_number;
    uint8_t end_bus_number;
    uint32_t reserved;
} acpi_mcfg_entry_t;
#pragma pack(pop)

int pci_ecam_init_from_mcfg(void* mcfg_table_virt, pci_ecam_map_fn_t mapper)
{
    if (!mcfg_table_virt || !mapper) return -EINVAL;

    acpi_mcfg_t* mcfg = (acpi_mcfg_t*) mcfg_table_virt;
    if (mcfg->hdr.length < sizeof(acpi_mcfg_t)) return -EINVAL;

    size_t entries_bytes = (size_t) mcfg->hdr.length - sizeof(acpi_mcfg_t);

    size_t count = entries_bytes / sizeof(acpi_mcfg_entry_t);
    acpi_mcfg_entry_t* ent = (acpi_mcfg_entry_t*) ((uint8_t*) mcfg+sizeof(acpi_mcfg_t));
    int installed = 0;
    for(size_t i =0; i < count; i++)
    {
        uint8_t start = ent[i].start_bus_number;
        uint8_t end = ent[i].end_bus_number;
        if (start > end) continue;

        size_t buses = (size_t) (end - start + 1);
        size_t map_sz = buses << PCI_ECAM_BUS_SHIFT; // 1 MB per bus
        uint64_t phys = ent[i].base_address + ((uint64_t) start << PCI_ECAM_BUS_SHIFT);
        void* v = mapper(phys, map_sz);
        if (!v) continue;

        if (pci_ecam_install_range(ent[i].pci_segment_group, phys, start, end, v) == 0)
        {
            installed++;
        }
    }

    return (installed > 0) ? 0 : -ENOENT;
}

bool pci_ecam_available(void)
{
    return g_ecam_enabled;
}

uint32_t pci_cfg_read_dword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
    uint8_t off = offset & ~0x3u;
    volatile uint8_t* p = pci_ecam_cfg_ptr(bus, slot, func, off);
    if (p) return *(volatile uint32_t*) p;

    uint32_t address = pci_cfg_addr_legacy(bus, slot, func, off);
    outdw(PCI_CFG_ADDRESS, address);
    return insdw(PCI_DATA_ADDRESS);
}

uint16_t pci_cfg_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
    uint8_t off = offset & ~0x3u;
    volatile uint8_t* p = pci_ecam_cfg_ptr(bus, slot, func, off);
    if (p)
    {
        uint32_t v = *(volatile uint32_t*) p;
        return (uint16_t) ((v >> ((offset & 2u) * 8))  & 0xFFFFu);
    }

    uint32_t address = pci_cfg_addr_legacy(bus, slot, func, off);
    outdw(PCI_CFG_ADDRESS, address);
    uint32_t v = insdw(PCI_DATA_ADDRESS);
    return (uint16_t) ((v >> ((offset & 2u) * 8))  & 0xFFFFu);
}

uint8_t pci_cfg_read_byte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
    uint32_t v = pci_cfg_read_dword(bus, slot, func, offset & ~0x3u);
    return (uint8_t) (v >> ((offset & 3u) * 8));
}

void pci_cfg_write_dword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t value)
{
    uint8_t off = offset & ~0x3u;
    volatile uint8_t* p = pci_ecam_cfg_ptr(bus, slot, func, off);
    if (p)
    {
        *(volatile uint32_t*)p = value;
        return;
    }

    uint32_t address = pci_cfg_addr_legacy(bus, slot, func, off);
    outdw(PCI_CFG_ADDRESS, address);
    outdw(PCI_DATA_ADDRESS, value);
}   


void pci_cfg_write_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint16_t value)
{
    uint8_t off = offset & ~0x3u;
    volatile uint8_t* p = pci_ecam_cfg_ptr(bus, slot, func, off);
    if (p)
    {
        volatile uint32_t* pdw = (volatile uint32_t*) p;
        uint32_t tmp = *pdw;
        if ((offset & 2u) == 0)
        {
            tmp = (tmp & 0xFFFF0000u) | (uint32_t) value;
        }
        else
        {
            tmp = (tmp & 0x0000FFFFu) | ((uint32_t) value << 16);
        }

        *pdw = tmp;
        return;
    }


    // Legacy PCI 
    uint32_t address = pci_cfg_addr_legacy(bus, slot, func, off);
    outdw(PCI_CFG_ADDRESS, address);
    uint32_t tmp = insdw(PCI_DATA_ADDRESS);
    if ((offset & 2u) == 0)
    {
        tmp = (tmp & 0xFFFF0000u) | (uint32_t) value;
    }
    else
    {
        tmp = (tmp & 0x0000FFFFu) | ((uint32_t) value << 16); 
    }

    outdw(PCI_CFG_ADDRESS, address);
    outdw(PCI_DATA_ADDRESS, tmp);
}

static void pci_size_bars(uint8_t bus, uint8_t dev, uint8_t func, struct pci_device* device)
{
    uint8_t hdr = pci_cfg_read_byte(bus, dev, func, PCI_HEADER_HEADER_TYPE_OFFSET) & 0X7Fu;
    int bar_count = (hdr == 0x00) ? 6 : (hdr == 0x01 ? 2 : 0); // type 0:6, type1:2, else: 0
    
    uint16_t cmd_orig = pci_cfg_read_word(bus, dev, func, PCI_HEADER_COMMAND_OFFSET);
    // Disable IO, mem and bus mastering during bar sizing 
    pci_cfg_write_word(bus, dev, func, PCI_HEADER_COMMAND_OFFSET, (uint16_t)(cmd_orig & ~(uint16_t) 0x0007));

    for (int i = 0; i < bar_count; ++i)
    {
        struct pci_device_bar* bar = &device->bars[i];
        bar->flags = 0;
        bar->addr = 0;
        bar->size = 0;
        uint8_t off = PCI_HEADER_BAR0_OFFSET + i * 4;

        uint32_t lo_orig = pci_cfg_read_dword(bus, dev, func, off);
        bool is_io = (lo_orig & 0x1u) != 0;
        bar->type = is_io ? PCI_DEVICE_IO_PORT : PCI_DEVICE_IO_MEMORY;
        if (!is_io && (lo_orig & 0x8u)) 
            bar->flags |= PCI_DEVICE_BAR_FLAG_PREFETCHABLE;

        if (is_io)
        {
            uint32_t base = lo_orig & ~0x3u;
            pci_cfg_write_dword(bus, dev, func, off, 0xFFFFFFFFu);
            uint32_t szv = pci_cfg_read_dword(bus, dev, func, off);
            pci_cfg_write_dword(bus, dev, func, off, lo_orig);

            uint32_t masked = (szv & ~0x3u);
            if (!masked)
            {
                bar->size = 0;
                continue;
            }

            uint64_t mask = (uint64_t) masked;
            bar->addr = (uint64_t) base;
            bar->size = (~mask) + 1u;
        }
        else
        {
            uint32_t type_bits = (lo_orig >> 1) & 0x3u;
            bool is64 = (type_bits == 0x2u);
            if (is64 && (i+1) < bar_count)
            {
                uint32_t hi_orig = pci_cfg_read_dword(bus, dev, func, off+4);

                pci_cfg_write_dword(bus, dev, func, off, 0xFFFFFFFFu);
                pci_cfg_write_dword(bus, dev, func, off+4, 0xFFFFFFFFu);
                uint32_t lo_sz = pci_cfg_read_dword(bus,dev, func, off);
                uint32_t hi_sz = pci_cfg_read_dword(bus, dev, func, off+4);
                pci_cfg_write_dword(bus, dev, func, off, lo_orig);
                pci_cfg_write_dword(bus, dev, func, off+4, hi_orig);

                uint64_t base64 = ((uint64_t) hi_orig << 32) | (uint64_t)(lo_orig & ~0xFu);
                uint64_t mask64 = ((uint64_t) hi_sz << 32) | (uint64_t)(lo_sz & ~0xFu);
                if (mask64 == 0)
                {
                    bar->size = 0;
                    goto restore64;
                }

                bar->addr = base64;
                bar->size = (~mask64) + 1u;
                bar->flags |= PCI_DEVICE_BAR_FLAG_64BIT;
restore64:
                ++i;
                if (i < bar_count)
                {
                    struct pci_device_bar* ext = &device->bars[i];
                    ext->type = bar->type;
                    ext->flags = PCI_DEVICE_BAR_FLAG_IS_EXT;
                    ext->addr = base64 >> 32;
                    ext->size = 0;
                }
            }
            else
            {
                uint32_t base = lo_orig & ~0xFu;
                pci_cfg_write_dword(bus, dev, func, off, 0xFFFFFFFFu);
                uint32_t szv = pci_cfg_read_dword(bus, dev, func, off);
                pci_cfg_write_dword(bus, dev, func, off, lo_orig);

                uint32_t masked = (szv & ~0xFu);
                if (!masked)
                {
                    bar->size = 0;
                    continue;
                }
                uint64_t mask = (uint64_t) masked;
                bar->addr = (uint64_t) base;
                bar->size = (~mask) + 1u;
            }
        }
    }

    pci_cfg_write_word(bus, dev, func, PCI_HEADER_COMMAND_OFFSET, cmd_orig);
}

static void pci_scan_bus(uint8_t bus, struct pci_device* parent_bridge)
{
    for (int dev = 0; dev < 32; ++dev)
    {
        uint16_t vendor0 = pci_cfg_read_word(bus, dev, 0, PCI_HEADER_VENDOR_OFFSET);
        if (vendor0 == 0xFFFF) continue;

        uint8_t hdr0 = pci_cfg_read_byte(bus, dev, 0, PCI_HEADER_HEADER_TYPE_OFFSET);
        int func_limit = (hdr0 & 0x80) ? 8 : 1; 
        for(int func = 0; func < func_limit; ++func)
        {
            uint16_t vendor = pci_cfg_read_word(bus, dev, func, PCI_HEADER_VENDOR_OFFSET);
            if (vendor == 0xFFFF) continue;

            struct pci_device* device = kzalloc(sizeof(*device));
            if (!device)
            {
                panic("Out of memory for PCI devices\n");
            }

            device->addr.bus = (uint8_t) bus;
            device->addr.slot = (uint8_t) dev;
            device->addr.func = (uint8_t) func;
            device->vendor = vendor;
            device->device_id = pci_cfg_read_word(bus, dev, func, PCI_HEADER_DEVICE_OFFSET);

            uint32_t classreg = pci_cfg_read_dword(bus, dev, func, PCI_HEADER_REVISION_ID_OFFSET);
            device->class.base = (classreg >> 24) & 0xFF;
            device->class.subclass = (classreg >> 16) & 0xFF;

            device->parent_bridge = parent_bridge;
            device->is_bridge = false;
            device->primary_bus = bus;
            device->secondary_bus = 0;
            device->subordinate_bus = 0;
            pci_size_bars(bus, dev, func, device);

            vector_push(pci_device_vector, &device);
            uint8_t hdr = pci_cfg_read_byte(bus, dev, func, PCI_HEADER_HEADER_TYPE_OFFSET) & 0x7Fu;
            if (hdr == 0x01)
            {
                device->is_bridge = true;
                device->primary_bus = pci_cfg_read_byte(bus, dev, func, PCI_BRIDGE_PRIMARY_BUS_OFFSET);
                device->secondary_bus = pci_cfg_read_byte(bus, dev, func, PCI_BRIDGE_SECONDARY_BUS_OFFSET);
                device->subordinate_bus = pci_cfg_read_byte(bus, dev, func, PCI_BRIDGE_SUBORDINATE_BUS_OFFSET);
                if (device->secondary_bus != 0 && device->secondary_bus <= device->subordinate_bus)
                {
                    pci_scan_bus(device->secondary_bus, device);
                }
            }
        }
    }
}

int pci_init(void)
{
    pci_device_vector = vector_new(sizeof(struct pci_device*), 16, 0);
    if (!pci_device_vector)
    {
        return -ENOMEM;
    }

    pci_scan_bus(0, NULL);
    return 0;
}

size_t pci_device_count()
{
    return vector_count(pci_device_vector);
}

int pci_device_base_class(struct pci_device* device)
{
    return device->class.base;
}

int pci_device_subclass(struct pci_device* device)
{
    return device->class.subclass;
}

int pci_device_get(size_t index, struct pci_device** device_out)
{
    if (index >= pci_device_count())
    {
        return -EOUTOFRANGE;
    }

    return vector_at(pci_device_vector, index, device_out, sizeof(*device_out));
}


static void pci_enable_upstream_path(struct pci_device* bridge, bool need_io, bool need_mem)
{
    for (struct pci_device* p = bridge; p; p = p->parent_bridge)
    {
        uint16_t cmd = pci_cfg_read_word(p->addr.bus, p->addr.slot, p->addr.func, PCI_HEADER_COMMAND_OFFSET);
        cmd |= 0x0004;  // BME bit
        if (need_mem)
        {
            cmd |= 0x0002; // MSE
        }

        if (need_io)
        {
            cmd |= 0x0001; // IOSE
        }

        pci_cfg_write_word(p->addr.bus, p->addr.slot, p->addr.func, PCI_HEADER_COMMAND_OFFSET, cmd);
    }
}

void pci_enable_bus_master(struct pci_device* d)
{
    bool need_mem = false;
    bool need_io = false;

    for (int i = 0; i < 6; ++i)
    {
        if (d->bars[i].size == 0)
        {
            continue;
        }

        if (d->bars[i].flags & PCI_DEVICE_BAR_FLAG_IS_EXT)
        {
            continue;
        }

        if (d->bars[i].type == PCI_DEVICE_IO_MEMORY)
        {
            need_mem = true;
        }
        else
        {
            need_io = true;
        }
    }

    uint16_t cmd = pci_cfg_read_word(d->addr.bus, d->addr.slot, d->addr.func, PCI_HEADER_COMMAND_OFFSET);
    if (need_mem)
    {
        cmd |= 0x0002;
    }

    if (need_io)
    {
        cmd |= 0x0001;
    }
    cmd |= 0x0004; // BME
    pci_cfg_write_word(d->addr.bus, d->addr.slot, d->addr.func, PCI_HEADER_COMMAND_OFFSET, cmd);

    if (d->parent_bridge)
    {
        pci_enable_upstream_path(d->parent_bridge, need_io, need_mem);
    }
}