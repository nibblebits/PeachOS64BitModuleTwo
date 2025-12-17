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
