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

#include "pata.h"
#include "status.h"
#include "disk/disk.h"
#include "io/io.h"
#include "io/pci.h"
#include "memory/heap/kheap.h"

int pata_address(int base, int offset)
{
    return base+offset;
}
static bool pata_pci_device_present(void)
{
    size_t total_pci = pci_device_count();
    for(size_t i = 0; i < total_pci; i++)
    {
        struct pci_device* device = NULL;
        if(pci_device_get(i, &device) < 0 || !device)
        {
            continue;
        }

        if(device->class.base == PATA_PCI_BASE_CLASS && device->class.subclass == PATA_PCI_SUBCLASS)
        {
            return true;
        }
    }

    return false;
}

struct pata_driver_private_data* pata_private_new(PATA_DISK_DRIVE type, struct disk* primary_disk)
{
    struct pata_driver_private_data* primary_drive_data = kzalloc(sizeof(struct pata_driver_private_data));
    if (!primary_drive_data)
    {
        return NULL;
    }

    primary_drive_data->disk_drive = PATA_PRIMARY_DRIVE;
    primary_drive_data->real_disk = primary_disk;
    return primary_drive_data;
}

int pata_disk_base_drive_address(struct disk* disk, int offset)
{
    int drive_address = PATA_INVALID_BASE_ADDRESS;
    struct pata_driver_private_data* private_data = (struct pata_driver_private_data*) disk_private_data_driver(disk);
    if (!private_data)
    {
        return PATA_INVALID_BASE_ADDRESS;
    }

    if (private_data->disk_drive == PATA_PRIMARY_DRIVE)
    {
        drive_address = PATA_PRIMARY_DRIVE_BASE_ADDRESS;
    }
    else if(private_data->disk_drive == PATA_SECONDARY_DRIVE)
    {
        drive_address = PATA_SECONDARY_DRIVE_BASE_ADDRESS;
    }

    return drive_address + offset;
}
int pata_disk_ctrl_drive_address(struct disk* disk, int offset)
{
    int drive_address = PATA_INVALID_BASE_ADDRESS;
    struct pata_driver_private_data* private_data = disk_private_data_driver(disk);
    if (!private_data)
    {
        return PATA_INVALID_BASE_ADDRESS;
    }

    if (private_data->disk_drive == PATA_PRIMARY_DRIVE)
    {
        drive_address = PATA_PRIMARY_DRIVE_CTRL_ADDRESS;
    }
    else if (private_data->disk_drive == PATA_SECONDARY_DRIVE)
    {
        drive_address = PATA_SECONDARY_DRIVE_CTRL_ADDRESS;
    }

    if  (drive_address == PATA_INVALID_BASE_ADDRESS)
    {
        return drive_address;
    }

    return drive_address + offset;
}
int pata_disk_read_sector(struct disk* disk, int lba, int total, void* buf)
{
    int base_address = pata_disk_base_drive_address(disk, 0x00);
    while(insb(pata_address(base_address, 0x07)) & 0x80)
    {
        // spin..
    }

    // select the drive
    outb(pata_address(base_address, 0x06), 0xE0 | ((lba >> 24) & 0x0F));

    // sector count
    outb(pata_address(base_address, 0x02), (unsigned char) total);

    // LBA LOW, MID, HIGH
    outb(pata_address(base_address, 0x03), (unsigned char)(lba & 0xff));
    outb(pata_address(base_address, 0x04), (unsigned char)(lba >> 8) & 0xff);
    outb(pata_address(base_address, 0x05), (unsigned char)(lba >> 16) & 0xff);

    // read sectors command 0x20
    outb(pata_address(base_address, 0x07), 0x20);

    unsigned short* ptr = (unsigned short*) buf;
    for (int i = 0; i < total; i++)
    {
        while(insb(pata_address(base_address, 0x07)) & 0x80)
        {
            // spin
        }

        // check for an error bit
        char status = insb(pata_address(base_address, 0x07));
        if (status & 0x01)
        {
            return -EIO;
        }

        // DRQ =1 spin and wait
        while(!(insb(pata_address(base_address, 0x07)) & 0x08))
        {
            // spin
        }

        // read one sector 256 = 512 bytes
        for (int word = 0; word < 256; word++)
        {
            *ptr++ = insw(pata_address(base_address, 0x00));
        }
    }

    return 0;
}
int pata_driver_read(struct disk* disk, unsigned int lba, int total_sectors, void* buf_out)
{
    int res = 0;
    struct disk* hardware_disk = disk_hardware_disk(disk);
    res = pata_disk_read_sector(hardware_disk, lba, total_sectors, buf_out);
    return res;
}
int pata_driver_write(struct disk* disk, unsigned int lba, int total_sectors, void* buf_in)
{
    int res = -EUNIMP;
    return res;
}

int pata_driver_mount(struct disk_driver* driver)
{
    int res = 0;
    if (!pata_pci_device_present())
    {
        res = -ENOENT;
        goto out;
    }

    struct pata_driver_private_data* primary_drive_data = pata_private_new(PATA_PRIMARY_DRIVE, NULL);
    if (!primary_drive_data)
    {
        res = -ENOMEM;
        goto out;
    }

    primary_drive_data->disk_drive = PATA_PRIMARY_DRIVE;
    res = disk_create_new(driver, NULL, PEACHOS_DISK_TYPE_REAL, 0, 0, KERNEL_PATA_SECTOR_SIZE, primary_drive_data, &primary_drive_data->real_disk);
    if  (res < 0)
    {
        goto out;
    }

    // TODO if you want: implement secondary drive.
out:
    return res;
}
void pata_driver_unmount(struct disk* disk)
{
    struct pata_driver_private_data* private_data = disk_private_data_driver(disk);
    if (private_data)
    {
        kfree(private_data);
    }
}
int pata_driver_loaded(struct disk_driver* driver)
{
    return 0;
}
void pata_driver_unloaded(struct disk_driver* driver)
{
    // do nothing..
}

int pata_driver_mount_partition(struct disk* disk, long starting_lba, long ending_lba, struct disk** disk_partition_out)
{
    int res = 0;

    struct pata_driver_private_data* private_data = pata_private_new(PATA_PARTITION_DRIVE, disk);
    if (!private_data)
    {
        res = -ENOMEM;
        goto out;
    }
    
    res = disk_create_new(disk->driver, disk, PEACHOS_DISK_TYPE_PARTITION, starting_lba, ending_lba, disk->sector_size, private_data, disk_partition_out);

out:    
    return res;
}
struct disk_driver pata_driver = {
    .name={"PATA"},
    .functions.loaded=pata_driver_loaded,
    .functions.unloaded=pata_driver_unloaded,
    .functions.read=pata_driver_read,
    .functions.write=pata_driver_write,
    .functions.mount=pata_driver_mount,
    .functions.unmount=pata_driver_unmount,
    .functions.mount_partition=pata_driver_mount_partition
};

struct disk_driver* pata_driver_init()
{
    return &pata_driver;
}