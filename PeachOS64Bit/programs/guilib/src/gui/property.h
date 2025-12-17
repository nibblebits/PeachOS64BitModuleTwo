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

#ifndef GUILIB_PROPERTY_H
#define GUILIB_PROPERTY_H

struct vector;
struct gui_element;
struct gui_event;
// Click handler for where there was a click on a gui element
typedef int(*GUI_ELEMENT_PROPERTY_CLICK_HANDLER)(struct gui_element* element, int x, int y);
typedef int (*GUI_ELEMENT_PROPERTY_MOVE_HANDLER)(struct gui_element* element, int x, int y);
typedef int (*GUI_ELEMENT_PROPERTY_ATTACHED)(struct gui_element* element);
typedef int (*GUI_ELEMENT_PROPERTY_UNATTACHED)(struct gui_element* element);
typedef int (*GUI_ELEMENT_PROPERTY_EVENT_HANDLER)(struct gui_element* element, struct gui_event* event);

/**
 * i.e scrollable proeprties could allow an element to be scrolled
 * allows you to capture all actions taken on a given element
 * elements can have multiple proeprties
 */
struct gui_element_property
{
    // The element the property is attached to.
    struct gui_element* element;

    struct
    {
        GUI_ELEMENT_PROPERTY_CLICK_HANDLER click;
        GUI_ELEMENT_PROPERTY_MOVE_HANDLER move;
        GUI_ELEMENT_PROPERTY_ATTACHED attached;
        GUI_ELEMENT_PROPERTY_UNATTACHED unattached;
        GUI_ELEMENT_PROPERTY_EVENT_HANDLER event;
    } listeners;
};

int gui_element_property_attach(struct gui_element* element, struct gui_element_property* property);
int gui_element_property_unattach(struct gui_element* element, struct gui_element_property* property);

void gui_element_property_listen_click(struct gui_element_property* property, GUI_ELEMENT_PROPERTY_CLICK_HANDLER click_handler);
void gui_element_property_listen_move(struct gui_element_property* property, GUI_ELEMENT_PROPERTY_MOVE_HANDLER move_handler);
void gui_element_property_listen_event(struct gui_element_property* property, GUI_ELEMENT_PROPERTY_EVENT_HANDLER event_handler);

#endif
