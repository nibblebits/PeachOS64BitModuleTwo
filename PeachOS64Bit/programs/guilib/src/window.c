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

/*
 * Copyright (C) 2025 Daniel McCarthy <daniel@dragonzap.com>
 * Part of the PeachOS Part Two Development Series.
 * https://github.com/nibblebits/PeachOS64BitCourse
 * https://github.com/nibblebits/PeachOS64BitModuleTwo
 * Licensed under the GNU General Public License version 2 (GPLv2).
 *
 * Community contributors to this source file:
 * NONE AS OF YET
 * ----------------
 * Disclaimer: Contributors are hobbyists that contributed to the public source code, they are not affiliated or endorsed by Daniel McCarthy the author of the PeachOS Kernel      
 * development video series. Contributors did not contribute to the video content or the teaching and have no intellectual property rights over the video content for the course video * material and did not contribute to the video material in anyway.
 */

#include "window.h"
#include "peachos.h"

void* window_graphics(struct window* window)
{
    return peachos_window_get_graphics(window);
}

void window_redraw(struct window* window)
{
    peachos_window_redraw(window);
}


void window_redraw_region(struct window* window, int rect_x, int rect_y, int rect_width, int rect_height)
{
    peachos_window_redraw_region(rect_x, rect_y, rect_width, rect_height, window);
}

struct window* window_create(const char* title, int width, int height, int flags, int id)
{
    // Call assembler function to creaate the window on the kernel.
    struct window* win = peachos_window_create(title, width, height, flags, id);
    return win;
}



void window_title_set(struct window* window, const char* title)
{
    peachos_window_title_set(window, title);
}

int window_get_event(struct window_event* event_out)
{
    return peachos_process_get_window_event(event_out);
}
void window_set_to_receive_stdout(struct window* win)
{
    peachos_divert_stdout_to_window(win);
}