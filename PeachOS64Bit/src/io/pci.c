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
