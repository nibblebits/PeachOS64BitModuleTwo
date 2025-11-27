#ifndef KERNEL_PCI_H
#define KERNEL_PCI_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define PCI_HEADER_VENDOR_OFFSET            0x00  // Vendor ID ( 16 bits)
#define PCI_HEADER_DEVICE_OFFSET            0x02 // Device ID (16 bits)
#define PCI_HEADER_COMMAND_OFFSET           0x04 // Command (16 bit)
#define PCI_HEADER_STATUS_OFFSET            0x06 // Status (16 bits)
#define PCI_HEADER_REVISION_ID_OFFSET       0x08 // Revision ID (8 bits)
#define PCI_HEADER_PROG_IF_OFFSET           0x09  // Programming interfac e8 bits
#define PCI_HEADER_SUBCLASS_OFFSET          0x0A // Subclass 8 bits
#define PCI_HEADER_CLASS_CODE_OFFSET        0x0B // class code 8 bits
#define PCI_HEADER_CACHE_LINE_SIZE_OFFSET   0x0C // cache line offset 8 bits
#define PCI_HEADER_LATENCY_TIMER_OFFSET     0x0D // Latency timer 8 bits
#define PCI_HEADER_HEADER_TYPE_OFFSET       0x0E // header type 8 bits
#define PCI_HEADER_BIST_OFFSET              0x0F // BIST 8 bits

#define PCI_HEADER_BAR0_OFFSET              0x10 // base address register 0
#define PCI_HEADER_BAR1_OFFSET              0x14 // base address register 1
#define PCI_HEADER_BAR2_OFFSET              0x18 // ...
#define PCI_HEADER_BAR3_OFFSET              0x1C
#define PCI_HEADER_BAR4_OFFSET              0x20
#define PCI_HEADER_BAR5_OFFSET              0x24


#define PCI_BRIDGE_PRIMARY_BUS_OFFSET       0x18
#define PCI_BRIDGE_SECONDARY_BUS_OFFSET     0x19
#define PCI_BRIDGE_SUBORDINATE_BUS_OFFSET   0x1A


#define PCI_HEADER_INTERRUPT_LINE_OFFSET    0x3C
#define PCI_HEADER_INTERRUPT_PIN_OFFSET     0x3D

#define PCI_CFG_ADDRESS                     0xCF8
#define PCI_DATA_ADDRESS                    0xCFC

#define PCI_BASE_CLASS(bus, slot, func)  pci_cfg_read_byte(bus, slot, PCI_HEADER_CLASS_CODE_OFFSET)
#define PCI_BASE_SUBCLASS(bus, slot, func) pci_cfg_read_byte(bus, slot, func, PCI_HEADER_SUBCLASS_OFFSET)

enum
{
    PCI_DEVICE_IO_MEMORY = 0,
    PCI_DEVICE_IO_PORT = 1,
};

enum
{
    PCI_DEVICE_BAR_FLAG_64BIT = 0x01,
    PCI_DEVICE_BAR_FLAG_IS_EXT = 0x02,
    PCI_DEVICE_BAR_FLAG_PREFETCHABLE = 0x04
};

struct pci_device_bar
{
    int type; // IO MEMORY OR IO PORT
    uint32_t flags;
    uint64_t addr; 
    uint64_t size; 
};

struct pci_address
{
    uint8_t bus;
    uint8_t slot; // aka dev
    uint8_t func;
};

struct pci_class_code
{
    int base;
    int subclass;
};

struct pci_device
{
    struct pci_address addr;
    struct pci_device_bar bars[6];

    uint16_t vendor;
    uint16_t device_id;
    struct pci_class_code class;

    bool is_bridge;
    uint8_t  primary_bus;
    uint8_t secondary_bus;
    uint8_t subordinate_bus;
    struct pci_device* parent_bridge;
};

// PCI Express
struct pci_ecam_range
{
    uint16_t seg_group;
    uint8_t start_bus;
    uint8_t end_bus;
    uint64_t phys_base;
    void* virt_base;
};

typedef void* (*pci_ecam_map_fn_t)(uint64_t phys, size_t size);
int pci_ecam_init_from_mcfg(void* mcfg_table_table_virt, pci_ecam_map_fn_t mapper);
int pci_ecam_install_range(uint16_t seg_group, uint64_t phys_base, uint8_t start_bus, uint8_t end_bus, void* virt_base);
bool pci_ecam_available(void);

size_t pci_device_count();
uint8_t pci_cfg_read_byte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint16_t pci_cfg_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint32_t pci_cfg_read_dword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
void pci_cfg_write_dword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t value);
void pci_cfg_write_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint16_t value);

int pci_init();
int pci_device_get(size_t index, struct pci_device** device_out);
int pci_device_base_class(struct pci_device* device);
int pci_device_subclass(struct pci_device* device);

void bus_enable_bus_master(struct pci_device* device);

#endif