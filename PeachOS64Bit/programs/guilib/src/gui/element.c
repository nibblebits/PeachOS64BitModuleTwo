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

#include "element.h"
#include "stdlib.h"
#include "memory.h"
#include "gui.h"
#include "vector.h"
#include "../graphics.h"
#include "gui.h"
#include "stdio.h"
#include "property.h"
#include "status.h"

struct framebuffer_pixel;

void gui_element_focus(struct gui_element *element);

GUI_EVENT_HANDLER_RESPONSE gui_element_event_handler(struct gui_event *gui_event)
{
    int res = 0;
    struct gui_element *element = gui_event->element.ptr;
    struct gui *gui = gui_event->gui;
    if (!element)
    {
        return GUI_EVENT_HANDLER_RESPONSE_IGNORED;
    }

    if (gui_event->type == GUI_EVENT_TYPE_ELEMENT_CLICKED)
    {
        // If we get clicked by default we shall also focus
        // I should move this somewhere else at some point
        // as overriding this handler loses the focus behaviour
        // maybe thats a good thing, i dont know yet
        // now we also focus.
        // Only focus if we arent already focused of course.
        if (gui_focused_element(gui) != element)
        {
            // Focus shall unfocus the older element as well
            gui_element_focus(element);
        }
    }

    return res;
}

void gui_element_draw_children(struct gui_element *element)
{
    size_t total_children = vector_count(element->children);
    for (size_t i = 0; i < total_children; i++)
    {
        // Loop through every child
        struct gui_element *child_elem = NULL;
        vector_at(element->children, i, &child_elem, sizeof(child_elem));
        if (child_elem)
        {
            // Draw the child.
            gui_element_draw(child_elem);
        }
    }
}
void gui_element_draw(struct gui_element *element)
{
    // Invalid..
    if (!element || !element->functions.draw)
    {
        return;
    }

    // Call the function. element shall draw its self
    element->functions.draw(element);

    // Let's now draw the childen, its a vector
    gui_element_draw_children(element);
}

void gui_element_draw_border(struct gui_element *gui_element, int border_width, struct framebuffer_pixel *color)
{
    // Top border
    gui_element_draw_rect(gui_element, 0, 0, gui_element->width, border_width, color);
    // Left border
    gui_element_draw_rect(gui_element, 0, 0, border_width, gui_element->height, color);

    // Bottom border
    gui_element_draw_rect(gui_element, 0, gui_element->height - border_width, gui_element->width, border_width, color);

    // Right border
    gui_element_draw_rect(gui_element, gui_element->width - border_width, 0, border_width, gui_element->height, color);
}

bool gui_element_bounds_check(struct gui_element *element, int x, int y)
{
    // Let's first check for relative overflow
    if (x < 0 || x >= element->width ||
        y < 0 || y >= element->height)
    {
        return false;
    }
    // Absolute overflow isnt possible if relative overflow
    // did not happen, this is because during the creation
    // process we ect.

    return true;
}

void gui_element_focus(struct gui_element *element)
{
    gui_focus_on_element(element->gui, element);
}
/**
 * Poorly named function, this function will check if a gui element
 * exists in the given position relative to the window position.
 */
bool gui_element_exists_in_position(struct gui_element *element, int x, int y)
{
    // Let's first check for relative overflow
    if (x < 0 || y < 0)
    {
        return false;
    }

    int starting_x = element->x;
    int starting_y = element->y;
    int ending_x = element->x + element->width;
    int ending_y = element->y + element->height;
    if (x >= starting_x && x <= ending_x &&
        y >= starting_y && y <= ending_y)
    {
        return true;
    }

    return false;
}

// TODO THink of a better name for that.
bool gui_element_should_redraw(struct gui_element *element)
{
    return element->flags & GUI_ELEMENT_REDRAW_REQUIRED;
}

void gui_element_mark_for_redraw(struct gui_element *element)
{
    element->flags |= GUI_ELEMENT_REDRAW_REQUIRED;
    // GUI Must be aware of the redraw.
    gui_mark_for_redraw(element->gui);
}

void gui_element_redrawn(struct gui_element *element)
{
    element->flags &= ~GUI_ELEMENT_REDRAW_REQUIRED;
}

void gui_element_draw_rect(struct gui_element *element, int x, int y, int width, int height, struct framebuffer_pixel *pixel_color)
{
    for (int lx = 0; lx < width; lx++)
    {
        for (int ly = 0; ly < height; ly++)
        {
            gui_element_draw_pixel(element, x + lx, y + ly, pixel_color);
        }
    }
}
/**
 * Make functions later to draw rects as it will be faster.
 */
void gui_element_draw_pixel(struct gui_element *element, int x, int y, struct framebuffer_pixel *pixel)
{
    // bounds failure refuse to draw.
    if (!gui_element_bounds_check(element, x, y))
    {
        return;
    }

    // true_x and true_y already have the absolute coordiantes
    // we can use them
    int true_x = x;
    int true_y = y;

    // If this element is a child then we need to also consider its true
    // relative x,y position to its parent
    if (element->parent)
    {
        // using ->x or ->y uses the actual offset from the start
        // of the window, completely incorrect to do that
        // will lead to an overflow.
        // now we use rel_root_x, rel_root_y
        // these are relative coordinates from our oldest grandparent
        true_x += element->rel_root_x;
        true_y += element->rel_root_y;
    }

    // Pixels will be drawn in wrong order due to the relative width
    // being different, we need to solve this
    // possibly will be fine actually, need thought on this
    // I believe the positoning is correct., test to be sure

    struct framebuffer_pixel *pixel_out = &(element->pixels[true_y * element->real_width + true_x]);
    memcpy(pixel_out, pixel, sizeof(struct framebuffer_pixel));
}
int gui_element_pixel_get(struct gui_element *element, int x, int y, struct framebuffer_pixel *pixel_out)
{
    if (!gui_element_bounds_check(element, x, y))
    {
        // bounds failure refuse to draw
        return -1;
    }

    // Bounds was fine get the pixel
    int true_x = element->x + x;
    int true_y = element->y + y;
    struct framebuffer_pixel *pixel_in = &element->pixels[true_y * element->real_width + true_x];
    *pixel_out = *pixel_in;

    return 0;
}

// one handler per element
// if you need special features use a property
void gui_element_event_handler_set(struct gui_element *element, GUI_EVENT_HANDLER_FUNCTION handler_func)
{
    element->functions.event_handler = handler_func;
}

struct gui_element *gui_element_create(struct gui *gui, struct gui_element *parent, int x, int y, int width, int height, GUI_ELEMENT_DRAW draw_function, GUI_ELEMENT_FREE free_function, int id)
{
    int res = 0;
    struct gui_element *element = calloc(1, sizeof(struct gui_element));
    if (!element)
    {
        res = -ENOMEM;
        goto out;
    }
    element->id = id;
    element->parent = parent;
    element->x = x;
    element->y = y;
    element->rel_x = x;
    element->rel_y = y;
    element->rel_root_x = 0;
    element->rel_root_y = 0;
    element->width = width;
    element->height = height;
    element->real_width = width;
    element->real_height = height;
    element->functions.draw = draw_function;
    element->functions.free = free_function;
    gui_element_event_handler_set(element, gui_element_event_handler);

    // No child yet..
    element->children = vector_new(sizeof(struct gui_element *), 8, 0);
    element->properties = vector_new(sizeof(struct gui_element_property *), 4, 0);

    element->gui = gui;

    // Create new graphics on kernel
    // only if theirs no parent
    // otherwise the child shall share the graphical entity
    if (!parent)
    {
        // The kernel graphic will be relative to the window
        // body
        element->graphics = graphics_create_relative(gui->win_graphics, x, y, width, height);
        if (!element->graphics)
        {
            res = -EIO;
            goto out;
        }
        element->pixels = graphics_get_pixel_buffer(element->graphics);
        element->flags |= GUI_ELEMENT_IS_ROOT_ELEMENT;

        // And push to the gui children
        vector_push(gui->elements, &element);
    }
    else
    {
        // Parent and child shall share the pixel buffers.
        element->graphics = parent->graphics;
        element->pixels = parent->pixels;
        // As we have a parent we need to adjust our position
        // as our x, y should be relative from the parents positioning
        element->x = parent->x + element->rel_x;
        element->y = parent->y + element->rel_y;
        element->rel_root_x = parent->rel_root_x + element->rel_x;
        element->rel_root_y = parent->rel_root_y + element->rel_y;

        // True widht/height = the parent with the maximum
        // width and  height, we use the same dimensions for safety
        element->real_width = parent->real_width;
        element->real_height = parent->real_height;
        // We must check for an overflow.
        // FIX THIS CODE LATER DONT THINK ITS TOTALLY RIGHT.
        // int cur_left_bounds = element->x + element->width;
        // int cur_top_bounds = element->y + element->height;
        // if (cur_left_bounds >= element->real_width)
        // {
        //     // We overflowed, lets adjust our local width to accomodate
        //     element->width -= (cur_left_bounds - element->real_width);
        //     if (element->width <= 0)
        //     {
        //         // Underflow unrecoverable throw error
        //         res = -1;
        //         goto out;
        //     }
        // }

        // if (cur_top_bounds >= element->real_height)
        // {
        //     element->height -= (cur_top_bounds-element->real_height);
        //     if (element->height <= 0)
        //     {
        //         // Underflow throw error
        //         res = -1;
        //         goto out;
        //     }
        // }

        // Width/height has been adjusted if need be to avoid
        // overflow.

        vector_push(parent->children, &element);
    }

    if (!element->pixels)
    {
        res = -ENOMEM;
        goto out;
    }

    // Element of course needs to be drawn
    // for the first time, or it wont be visible.
    gui_element_mark_for_redraw(element);

out:
    if (res < 0)
    {
        if (element)
        {
            // TODO free graphics...
            if (parent)
            {
                vector_pop_element(parent->children, &element, sizeof(element));
            }
            else
            {
                vector_pop_element(gui->elements, &element, sizeof(element));
            }
            free(element);
            element = NULL;
        }
    }
    return element;
}

void gui_element_free(struct gui_element *element)
{
    // Call the inner free function so private can be freed
    if (element->functions.free)
    {
        element->functions.free(element);
    }

    // Now fre ourselves
    free(element);

    // dONE..
}

void gui_element_private_set(struct gui_element *element, void *private)
{
    element->private = private;
}

struct gui_element *gui_element_parent(struct gui_element *element)
{
    return element->parent;
}

struct gui_element *gui_element_root(struct gui_element *element)
{
    struct gui_element *current_element = element;
    struct gui_element *last_element = element;
    while (current_element)
    {
        last_element = current_element;
        current_element = gui_element_parent(current_element);
    }

    // Root most element is the last valid element.
    // this iss the top mostp arent
    // essential to know this for the vector
    return last_element;
}
