#ifndef KERNEL_DISK_DRIVERS_PATA_H
#define KERNEL_DISK_DRIVERS_PATA_H

#include "disk.h"
#include "driver.h"

#define KERNEL_PATA_SECTOR_SIZE 512
#define PATA_PCI_BASE_CLASS 0x01
#define PATA_PCI_SUBCLASS 0x01

#define PATA_INVALID_BASE_ADDRESS -1
#define PATA_PRIMARY_DRIVE_BASE_ADDRESS 0x1F0
#define PATA_SECONDARY_DRIVE_BASE_ADDRESS 0x170
#define PATA_PRIMARY_DRIVE_CTRL_ADDRESS 0x3F6
#define PATA_SECONDARY_DRIVE_CTRL_ADDRESS 0x376

enum
{
    PATA_INVALID_DRIVE = -1,
    PATA_PRIMARY_DRIVE = 0,
    PATA_SECONDARY_DRIVE = 1,
    PATA_PARTITION_DRIVE = 2
};

typedef int PATA_DISK_DRIVE;
struct disk;
struct pata_driver_private_data
{
    PATA_DISK_DRIVE disk_drive;
    struct disk* real_disk;
};

struct disk_driver* pata_driver_init();
#endif