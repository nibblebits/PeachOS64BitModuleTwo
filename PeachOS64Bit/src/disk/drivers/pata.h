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