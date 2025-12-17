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

#include "plane.h"
#include "stdlib.h"
#include "memory.h"
#include "element.h"
#include "graphics.h"
#include "stdio.h"
#include "gui.h"

void gui_element_plane_flag_set(struct gui_element *gui_element, int flag)
{
    struct userland_element_plane_private *private_data = gui_element->private;
    private_data->flags |= flag;
}

bool gui_element_plane_should_draw_border(struct gui_element *element)
{
    struct userland_element_plane_private *private_data = element->private;
    return private_data->flags & USERLAND_PLANE_FLAG_DRAW_STANDARD_BORDER;
}

bool gui_element_plane_draw_border_on_focus(struct gui_element *element)
{
    struct userland_element_plane_private *private_data = element->private;
    return private_data->flags & USERLAND_PLANE_FLAG_DRAW_BORDER_ON_FOCUS;
}

GUI_EVENT_HANDLER_RESPONSE gui_element_plane_event_handler(struct gui_event *gui_event)
{
    switch (gui_event->type)
    {
    case GUI_EVENT_TYPE_ELEMENT_FOCUSED:
    case GUI_EVENT_TYPE_ELEMENT_UNFOCUSED:

        // We have a focus event great.. THEN WE must redraw ourselves
        // because our border shall change based on the focused element
        gui_element_mark_for_redraw(gui_event->element.ptr);
        break;
    }

    return GUI_EVENT_HANDLER_RESPONSE_PROCESSED_CONTINUE_WITH_CHILDREN;
}

void gui_element_plane_draw(struct gui_element *gui_element)
{
    struct userland_element_plane_private *plane = gui_element->private;

    // Draw the colors/background..
    struct framebuffer_pixel pixel = {0};
    pixel.red = plane->bg.red;
    pixel.green = plane->bg.green;
    pixel.blue = plane->bg.blue;
    gui_element_draw_rect(gui_element, 0, 0, gui_element->width, gui_element->height, &pixel);

    // Are we the focused element? if so we shall need a border
    if (gui_element_plane_draw_border_on_focus(gui_element) &&
        gui_focused_element(gui_element->gui) == gui_element)
    {
        // Yes we are the focused element draw the border
        // The border shall be drawn in four places
        // The color is the focused_border_color
        gui_element_draw_border(gui_element, plane->focused_border.width, &plane->focused_border.color);
    }
    else if (gui_element_plane_should_draw_border(gui_element))
    {
        // Not focused? we still have a border but a different colour
        gui_element_draw_border(gui_element, plane->border.width, &plane->border.color);
    }
}

void gui_element_plane_free(struct gui_element *gui_element)
{
    // Freee the private data.
    free(gui_element->private);
}

void gui_element_plane_set_border_color(struct gui_element *gui_element, int red, int green, int blue)
{
    struct userland_element_plane_private *plane = gui_element->private;
    struct framebuffer_pixel color = {0};
    color.red = red;
    color.blue = blue;
    color.green = green;
    plane->border.color = color;
}

void gui_element_plane_set_border_width(struct gui_element *gui_element, int width)
{
    struct userland_element_plane_private *plane = gui_element->private;
    plane->border.width = width;
}

void gui_element_plane_set_focused_border_color(struct gui_element *gui_element, int red, int green, int blue)
{
    struct userland_element_plane_private *plane = gui_element->private;
    struct framebuffer_pixel color = {0};
    color.red = red;
    color.blue = blue;
    color.green = green;
    plane->focused_border.color = color;
}
void gui_element_plane_bg_color_set(struct gui_element *gui_element, int red, int green, int blue)
{
    struct userland_element_plane_private *plane = gui_element->private;
    plane->bg.red = red;
    plane->bg.green = green;
    plane->bg.blue = blue;
}

struct framebuffer_pixel gui_element_plane_bg_color_get(struct gui_element* gui_element)
{
    struct userland_element_plane_private* plane = gui_element->private;
    struct framebuffer_pixel bg_pixel = {.red=plane->bg.red,.green=plane->bg.green,.blue=plane->bg.blue,.reserved=0};
    return bg_pixel;
}

void gui_element_plane_focused_border_width_set(struct gui_element *gui_element, int pixel_width)
{
    struct userland_element_plane_private *plane = gui_element->private;
    plane->focused_border.width = pixel_width;
}
struct gui_element *gui_element_plane_create(struct gui *gui, struct gui_element *parent, int x, int y, int width, int height, int id)
{
    struct gui_element *element = gui_element_create(gui, parent, x, y, width, height, gui_element_plane_draw, gui_element_plane_free, id);
    if (!element)
    {
        // No memory left
        return NULL;
    }

    // Let's create the private data for the plane
    struct userland_element_plane_private *private = calloc(1, sizeof(struct userland_element_plane_private));
    if (!private)
    {
        // out of memory
        return NULL;
    }

    private->element = element;

    // Set the private.
    gui_element_private_set(element, private);

    // Default will allow border on focus.
    gui_element_plane_flag_set(element, USERLAND_PLANE_FLAG_DRAW_BORDER_ON_FOCUS);

    // Default wll allow border always
    gui_element_plane_flag_set(element, USERLAND_PLANE_FLAG_DRAW_STANDARD_BORDER);

    // We need to handle our own events for this plane
    gui_element_event_handler_set(element, gui_element_plane_event_handler);

    // Default will allow border on focus.
    gui_element_plane_focused_border_width_set(element, 1);

    // Default border width
    gui_element_plane_set_border_width(element, 1);

    // For focused border color we shall go for a red
    // its going to be ugly but distict for now.

    gui_element_plane_set_focused_border_color(element, 0xff, 0x00, 0x00);

    // default border color shall be a solid black for non-focused
    gui_element_plane_set_border_color(element, 0x00, 0x00, 0x00);

    return element;
}