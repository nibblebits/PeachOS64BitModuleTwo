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

#ifndef  KERNEL_PS2_MOUSE_H
#define KERNEL_PS2_MOUSE_H

#include "mouse/mouse.h"
#include <stdint.h>

#define PS2_COMMUNICATION_PORT 0x60
#define ISR_MOUSE_INTERRUPT 0x2c
#define PS2_STATUS_PORT 0x64
#define PS2_COMMAND_ENABLE_SECOND_PORT 0xA8
#define PS2_WRITE_TO_MOUSE 0xD4
#define PS2_READ_CONFIG_BYTE 0x20
#define PS2_UPDATE_CONFIG_BYTE 0x60
#define PS2_MOUSE_ENABLE_PACKET_STREAMING 0xF4
#define PS2_WAIT_FOR_INPUT_TO_CLEAR 0
#define PS2_WAIT_FOR_OUTPUT_TO_BE_SET 1

#define PS2_MOUSE_RESET 0xFF
#define PS2_MOUSE_ACK 0xFA
#define PS2_MOUSE_SELF_TEST_PASS 0xAA

#define PS2_STANDARD_MOUSE_DEVICE_ID 0x00
#define PS2_SCROLL_WHEEL_MOUSE_DEVICE_ID 0x03

struct ps2_mouse
{
    uint8_t device_id;
    int mouse_packet_size;
};

struct mouse* ps2_mouse_get();

#endif