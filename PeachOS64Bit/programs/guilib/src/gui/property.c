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

#include "property.h"

#include "element.h"
#include "gui.h"
#include "memory.h"
#include "stdlib.h"
#include "status.h"
#include "vector.h"
bool gui_element_property_has(struct gui_element *element, struct gui_element_property *property)
{
    size_t total_properties = vector_count(element->properties);
    for (size_t i = 0; i < total_properties; i++)
    {
        struct gui_element_property *current_property = NULL;
        vector_at(element->properties, i, &current_property, sizeof(current_property));
        if (current_property && (void*)current_property->listeners.attached == (void*)current_property->listeners.attached)
        {
            return true;
        }
    }

    return false;
}

int gui_element_property_attach(struct gui_element *element, struct gui_element_property *property)
{
    int res = 0;
    struct gui_element_property *cloned_property = NULL;
    if (!property->listeners.attached)
    {
        res = -EINVARG;
        goto out;
    }

    if (gui_element_property_has(element, property))
    {
        res = -EISTKN;
        goto out;
    }

    // We have to clone the property bcause it might of been on the stack or global
    cloned_property = calloc(1, sizeof(struct gui_element_property));
    if (!cloned_property)
    {
        res = -ENOMEM;
        goto out;
    }

    memcpy(cloned_property, property, sizeof(struct gui_element_property));
    cloned_property->element = element;

    res = property->listeners.attached(element);
    if (res < 0)
    {
        goto out;
    }
    vector_push(element->properties, &property);
out:

    if (res < 0)
    {
        if (cloned_property)
        {
            free(cloned_property);
        }
    }
    return res;
}

int gui_element_property_unattach(struct gui_element *element, struct gui_element_property *property)
{
    int res = 0;
    if (!gui_element_property_has(element, property))
    {
        res = -ENOENT;
        goto out;
    }

    if (property->listeners.unattached)
    {
        res = property->listeners.unattached(element);
        if (res < 0)
        {
            goto out;
        }
    }
    vector_pop_element(element->properties, &property, sizeof(property));
out:
    return 0;
}

void gui_element_property_listen_click(struct gui_element_property* property, GUI_ELEMENT_PROPERTY_CLICK_HANDLER click_handler)
{
    property->listeners.click = click_handler;
}


void gui_element_property_listen_move(struct gui_element_property* property, GUI_ELEMENT_PROPERTY_MOVE_HANDLER move_handler)
{
    property->listeners.move = move_handler;
}

void gui_element_property_listen_event(struct gui_element_property* property, GUI_ELEMENT_PROPERTY_EVENT_HANDLER event_handler)
{
    property->listeners.event = event_handler;
}
