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

#include "gui.h"
#include "memory.h"
#include "stdlib.h"
#include "stdio.h"
#include "window.h"
#include "element.h"
#include "event.h"
#include "vector.h"
#include "status.h"
#include <stdbool.h>

struct gui_element *gui_element_root_parent(struct gui_element *element)
{
    struct gui_element *current_elem = element;
    while (current_elem)
    {
        if (!current_elem->parent)
        {
            break;
        }

        current_elem = current_elem->parent;
    }

    return current_elem;
}
bool gui_element_exists_in_root(struct gui *gui, struct gui_element *element)
{
    struct gui_element *root_elem = gui_element_root_parent(element);
    // get total children
    size_t total_children = vector_count(gui->elements);
    for (size_t i = 0; i < total_children; i++)
    {
        struct gui_element *current_elem = NULL;
        vector_at(gui->elements, i, &current_elem, sizeof(current_elem));

        if (current_elem && current_elem == root_elem)
        {
            return true;
        }
    }

    return false;
}

struct gui_element* gui_focused_element(struct gui* gui)
{
    return gui->focused_element;
}

void gui_unfocus_element(struct gui* gui)
{
    struct gui_element* old_focused_elem = gui_focused_element(gui);
    if (!old_focused_elem)
    {
        // We had no such focused element in the past
        return;
    }

    // Unfocus the element and dispatch event.
    gui->focused_element = NULL;
    gui_event_push_event_element_unfocus(gui, old_focused_elem);
}

void gui_focus_on_element(struct gui* gui, struct gui_element* element)
{
    // Its not enough to focus on a new element
    // We shall first unfocus the old one
    gui_unfocus_element(gui);

    if (gui_focused_element(gui) == element)
    {
        // We are alread yfocused, we cant proceed
        // or infinite loop may happen
        return;
    }

    // Proceed with updating the focused element.
    gui->focused_element = element;
    // Let's push this event
    gui_event_push_event_element_focus(gui, element);
}
int gui_redraw(struct gui *gui)
{
    int res = 0;
    
    // The region to redraw

    // Get the window
    struct window *win = gui->window;
    // Loop through all elements and call their redraw
    size_t total_elements = vector_count(gui->elements);
    struct gui_element *element = NULL;
    for (size_t i = 0; i < total_elements; i++)
    {
        vector_at(gui->elements, i, &element, sizeof(element));
        if (element && gui_element_should_redraw(element))
        {
            gui_element_draw(element);
             // Let's redraw the rectangle of the element
            window_redraw_region(win, element->x, element->y, element->real_width, element->real_height);
        }
    }

    return res;
}
int gui_add_element(struct gui *gui, struct gui_element *element)
{
    if (gui_element_exists_in_root(gui, element))
    {
        // We already have this element.
        return -1;
    }

    // Get the top most element incase the element provided
    // was a child, i.e a button whos parent is a plane.
    // we must push elements only with NULL parents.
    struct gui_element *root_element = gui_element_root(element);
    if (!root_element)
    {
        return -1;
    }

    // Push the element to the GUI so its visible.
    vector_push(gui->elements, &root_element);

    // Redraw the scene
    // FUTURE: HAVE REGION REDRAWING, YET MAYBE THE KERNEL ALREADY
    // HANDLES THAT..
    gui_redraw(gui);
    return 0;
}
struct gui *gui_bind_to_window(struct window *window, GUI_EVENT_HANDLER_FUNCTION event_handler_func)
{
    struct gui *gui = calloc(1, sizeof(struct gui));
    if (!gui)
    {
        // out of mem
        return NULL;
    }

    gui->window = window;
    gui->win_graphics = window_graphics(window);
    gui->handlers.event = event_handler_func;
    gui->elements = vector_new(sizeof(struct gui_element *), 8, 0);
    gui->gui_events = vector_new(sizeof(struct gui_event*), 4, 0);
    if (!gui->elements)
    {
        // Vector issue.
        return NULL;
    }

    gui_mark_for_redraw(gui);
    return gui;
}

void gui_mark_for_redraw(struct gui *gui)
{
    gui->flags |= GUI_FLAG_MUST_DRAW;
}

int gui_process_event_focus(struct gui* gui, struct window_event* win_event)
{
    return 0;
}

int gui_process_event_lost_focus(struct gui* gui, struct window_event* win_event)
{
    return 0;
}

int gui_process_event_mouse_move(struct gui* gui, struct window_event* win_event)
{
    return 0;
}


int gui_process_event_mouse_click(struct gui* gui, struct window_event* win_event)
{
    return gui_event_push_event_mouse_click(gui, win_event->data.click.x, win_event->data.click.y, GUI_LEFT_CLICK);
}

int gui_process_event_window_close(struct gui* gui, struct window_event* win_event)
{
    return 0;
}

int gui_process_event_keypress(struct gui* gui, struct window_event* win_event)
{
    // Let's get the current element thats focused
    // and dispatch the keyboard presses to it

    struct gui_element* focused_elem = gui_focused_element(gui);
    if (!focused_elem)
    {
        return 0;
    }

    struct gui_event keypress_event = {0};
    keypress_event.type = GUI_EVENT_TYPE_KEYSTROKE;
    keypress_event.data.keystroke.key = win_event->data.keypress.key;

    // Better to call this function than interate
    // with a condition over the whole list.
    gui_element_event_clone_push(focused_elem, &keypress_event);
    
    return 0;
}
int gui_process_event(struct gui* gui, struct window_event *win_event)
{
    int res = 0;
    // We have a window event, lets see if we can handle it
    switch(win_event->type)
    {
        case WINDOW_EVENT_TYPE_FOCUS:
            res = gui_process_event_focus(gui, win_event);
        break;

        case WINDOW_EVENT_TYPE_LOST_FOCUS:
            res = gui_process_event_lost_focus(gui, win_event);
        break;

        case WINDOW_EVENT_TYPE_MOUSE_MOVE:
            res = gui_process_event_mouse_move(gui, win_event);
        break;

        case WINDOW_EVENT_TYPE_MOUSE_CLICK:
            res = gui_process_event_mouse_click(gui, win_event);
        break;

        case WINDOW_EVENT_TYPE_WINDOW_CLOSE:
            res = gui_process_event_window_close(gui, win_event);
        break;

        case WINDOW_EVENT_TYPE_KEY_PRESS:
            res = gui_process_event_keypress(gui,win_event);
        break;
    }

    return res;
}

void gui_redraw_if_required(struct gui *gui)
{
    if (gui->flags & GUI_FLAG_MUST_DRAW)
    {
        gui_redraw(gui);
        gui->flags &= ~GUI_FLAG_MUST_DRAW;
    }
}

int gui_process_events(struct gui *gui)
{
    int res = 0;
    // Lets's check if theres any events
    struct window_event event = {0};
    while (1)
    {
        res = window_get_event(&event);
        if (res < 0)
        {
            if (res == -EOUTOFRANGE)
            {
                // Out of range is not an error in terms of event processing.
                // means theres no events.
                res = 0;
            }
            break;
        }

        // We have an event, lets process it
        // kernel events not to be mistaken with the gui events in this SDK 
        res = gui_process_event(gui, &event);
        if (res < 0)
        {
            // err
            goto out;
        }

        // Let's now process the actual stored gui events, SDK events..
        gui_element_events_process(gui);
    }

out:
    return res;
}
int gui_process(struct gui *gui)
{
    int res = 0;
    gui_redraw_if_required(gui);

    // Is there any events that we need to process?
    res = gui_process_events(gui);
    if (res < 0)
    {
        goto out;
    }

out:
    return res;
}

int gui_draw(struct gui *gui)
{
    int res = 0;
    // These are root elements only
    size_t total = vector_count(gui->elements);
    for (size_t i = 0; i < total; i++)
    {
        struct gui_element *element = NULL;
        vector_at(gui->elements, i, &element, sizeof(element));
        if (element)
        {
            // Call the draw function on the element.
            gui_element_draw(element);
        }
    }
    return res;
}
