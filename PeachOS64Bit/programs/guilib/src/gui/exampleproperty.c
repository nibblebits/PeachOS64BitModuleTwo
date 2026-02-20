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

#include "exampleproperty.h"
#include "property.h"
#include "event.h"
#include "stdio.h"


int gui_element_property_example_property_click(struct gui_element* element, int x , int y);
int gui_element_property_example_property_unattached(struct gui_element* element);
int gui_element_property_example_property_attached(struct gui_element* element);
int gui_element_property_example_property_event(struct gui_element* element, struct gui_event* event);

struct gui_element_property example_property = {
    .listeners.attached=gui_element_property_example_property_attached,
    .listeners.click=gui_element_property_example_property_click,
    .listeners.move=0,
    .listeners.unattached=gui_element_property_example_property_unattached,
    .listeners.event=gui_element_property_example_property_event
};

int gui_element_property_example_property_event(struct gui_element* element, struct gui_event* event)
{
    switch(event->type)
    {
        case GUI_EVENT_TYPE_LISTAREA_ITEM_SELECTED:
            printf("Listarea selection: element %i selected\n", event->data.listarea_item_selected.selected_index);
        break;
    }

    return 0;
}


int gui_element_property_example_property_click(struct gui_element* element, int x , int y)
{
    printf("THe element was clicked at %i, %i\n", x, y);
    return 0;
}

int gui_element_property_example_property_unattached(struct gui_element* element)
{
    printf("Example property was unattached\n");
    return 0;
}
int gui_element_property_example_property_attached(struct gui_element* element)
{
    printf("Example propoerty was attached to element\n");
    return 0;
}

int gui_element_property_example_property_attach(struct gui_element* element)
{
    return gui_element_property_attach(element, &example_property);
}
