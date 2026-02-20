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

#include "userlandptr.h"
#include "memory/heap/kheap.h"
#include "lib/vector/vector.h"
#include "task/process.h"
struct userland_ptr* process_userland_pointer_create(struct process* process, void* kernel_ptr)
{
    struct userland_ptr* userland_ptr = kzalloc(sizeof(struct userland_ptr));
    if (!userland_ptr)
    {
        return NULL;
    }

    userland_ptr->kernel_ptr = kernel_ptr;
    vector_push(process->kernel_userland_ptrs_vector, &userland_ptr);
    return userland_ptr;
}

void process_userland_pointer_release(struct process* process, void* userland_ptr)
{
    if (!process_userland_pointer_registered(process, userland_ptr))
    {
        return;
    }

    vector_pop_element(process->kernel_userland_ptrs_vector, &userland_ptr, sizeof(userland_ptr));
    kfree(userland_ptr);
}

bool process_userland_pointer_registered(struct process* process, void* userland_ptr)
{
    size_t total_userland_ptrs = vector_count(process->kernel_userland_ptrs_vector);
    for(size_t i = 0; i < total_userland_ptrs; i++)
    {
        struct userland_ptr* current_ptr = NULL;
        vector_at(process->kernel_userland_ptrs_vector, i, &current_ptr, sizeof(current_ptr));
        if (current_ptr && current_ptr == userland_ptr)
        {
            return true;
        }
    }

    return false;
}


void* process_userland_pointer_kernel_ptr(struct process* process, void* userland_ptr)
{
    if (!process_userland_pointer_registered(process, userland_ptr))
    {
        return NULL;
    }

    struct userland_ptr*  _userland_ptr = (struct userland_ptr*) userland_ptr;
    return _userland_ptr->kernel_ptr;
}