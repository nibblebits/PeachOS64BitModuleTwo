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

#include "scrollable.h"
#include "memory.h"
#include "stdlib.h"

#include "gui/element.h"
#include "status.h"
typedef void(*GUI_ELEMENT_DRAW)(struct gui_element* elem);
typedef void(*GUI_ELEMENT_FREE)(struct gui_element* elem);

void gui_element_scrollable_draw(struct gui_element* elem)
{
    // Draw todo later..
}

void gui_element_scrollable_free(struct gui_element* elem)
{
    struct gui_scrollable_element_private* private_data = elem->private;
    if (!private_data)
        return;

    
    free(private_data);
}
struct gui_element *gui_element_scrollable_create(struct gui *gui, struct gui_element *parent, int x, int y, int width, int height, int id)
{
    int res = 0;
    struct gui_element* element = gui_element_create(gui, parent, x, y, width, height, gui_element_scrollable_draw, gui_element_scrollable_free, id);
    if (!element)
    {
        res = -ENOMEM;
        goto out;
    }

    struct gui_scrollable_element_private* private_data = calloc(1, sizeof(struct gui_scrollable_element_private));
    if (!private_data)
    {
        res = -ENOMEM;
        goto out;
    }

    private_data->scroll_y = 0;

out:
    if (res < 0)
    {
        if (element->private)
        {
            free(element->private);
            element->private = NULL;
        }
        gui_element_free(element);
        element = NULL;
    }
    return element;
}