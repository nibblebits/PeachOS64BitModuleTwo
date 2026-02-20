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

#ifndef ISR80H_WINDOW_H
#define ISR80H_WINDOW_H

enum
{
    ISR80H_WINDOW_UPDATE_TITLE = 0
};
struct interrupt_frame;
void* isr80h_command16_window_create(struct interrupt_frame* frame);
void* isr80h_command17_sysout_to_window(struct interrupt_frame* frame);
void* isr80h_command18_get_window_event(struct interrupt_frame* frame);
void* isr80h_command19_window_graphics_get(struct interrupt_frame* frame);
void* isr80h_command21_window_redraw(struct interrupt_frame* frame);
void* isr80h_command23_window_redraw_region(struct interrupt_frame* frame);
void* isr80h_command24_update_window(struct interrupt_frame* frame);
#endif