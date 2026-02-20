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

#ifndef KERNEL_USERLAND_PTR_H
#define KERNEL_USERLAND_PTR_H

#include <stdbool.h>
#include <stdint.h>

struct userland_ptr
{
    void* kernel_ptr;
};

struct process;
struct userland_ptr* process_userland_pointer_create(struct process* process, void* kernel_ptr);
void process_userland_pointer_release(struct process* process, void* userland_ptr);
bool process_userland_pointer_registered(struct process* process, void* userland_ptr);
void* process_userland_pointer_kernel_ptr(struct process* process, void* userland_ptr);

#endif