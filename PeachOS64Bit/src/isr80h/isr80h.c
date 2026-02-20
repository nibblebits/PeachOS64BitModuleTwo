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

#include "isr80h.h"
#include "idt/idt.h"
#include "misc.h"
#include "io.h"
#include "heap.h"
#include "process.h"
#include "file.h"
#include "window.h"
#include "graphics.h"
#include "time.h"
void isr80h_register_commands()
{
    isr80h_register_command(SYSTEM_COMMAND0_SUM, isr80h_command0_sum);
    isr80h_register_command(SYSTEM_COMMAND1_PRINT, isr80h_command1_print);
    isr80h_register_command(SYSTEM_COMMAND2_GETKEY, isr80h_command2_getkey);
    isr80h_register_command(SYSTEM_COMMAND3_PUTCHAR, isr80h_command3_putchar);
    isr80h_register_command(SYSTEM_COMMAND4_MALLOC, isr80h_command4_malloc);
    isr80h_register_command(SYSTEM_COMMAND5_FREE, isr80h_command5_free);
    isr80h_register_command(SYSTEM_COMMAND6_PROCESS_LOAD_START, isr80h_command6_process_load_start);
    isr80h_register_command(SYSTEM_COMMAND7_INVOKE_SYSTEM_COMMAND, isr80h_command7_invoke_system_command);
    isr80h_register_command(SYSTEM_COMMAND8_GET_PROGRAM_ARGUMENTS, isr80h_command8_get_program_arguments);
    isr80h_register_command(SYSTEM_COMMAND9_EXIT, isr80h_command9_exit);
    isr80h_register_command(SYSTEM_COMMAND10_FOPEN, isr80h_command10_fopen);
    isr80h_register_command(SYSTEM_COMMAND11_FCLOSE, isr80h_command11_fclose);
    isr80h_register_command(SYSTEM_COMMAND12_FREAD, isr80h_command12_fread);
    isr80h_register_command(SYSTEM_COMMAND13_FSEEK, isr80h_command13_fseek);
    isr80h_register_command(SYSTEM_COMMAND14_FSTAT, isr80h_command14_fstat);
    isr80h_register_command(SYSTEM_COMMAND15_REALLOC, isr80h_command15_realloc);
    isr80h_register_command(SYSTEM_COMMAND16_WINDOW_CREATE, isr80h_command16_window_create);
    isr80h_register_command(SYSTEM_COMMAND17_SYSOUT_TO_WINDOW, isr80h_command17_sysout_to_window);
    isr80h_register_command(SYSTEM_COMMAND18_GET_WINDOW_EVENT, isr80h_command18_get_window_event);
    isr80h_register_command(SYSTEM_COMMAND19_WINDOW_GRAPHICS_GET, isr80h_command19_window_graphics_get);
    isr80h_register_command(SYSTEM_COMMAND20_GRAPHICS_PIXELS_BUFFER_GET, isr80h_command20_graphics_pixels_get);
    isr80h_register_command(SYSTEM_COMMAND21_WINDOW_REDRAW, isr80h_command21_window_redraw);
    isr80h_register_command(SYSTEM_COMMAND22_GRAPHICS_CREATE, isr80h_command22_graphics_create);
    isr80h_register_command(SYSTEM_COMMAND23_WINDOW_REDRAW_REGION, isr80h_command23_window_redraw_region);
    isr80h_register_command(SYSTEM_COMMAND24_UPDATE_WINDOW, isr80h_command24_update_window);
    isr80h_register_command(SYSTEM_COMMAND25_UDELAY, isr80h_command25_udelay);
}