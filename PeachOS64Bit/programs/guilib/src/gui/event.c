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

#include "event.h"
#include "gui.h"
#include "element.h"
#include "stdlib.h"
#include "memory.h"
#include "status.h"
#include "stdio.h"
#include "property.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

struct gui_event *gui_event_new()
{
    return calloc(1, sizeof(struct gui_event));
}

void gui_event_free(struct gui_event *gui_event)
{
    free(gui_event);
}

struct gui_event *gui_event_clone(struct gui_event *event)
{
    struct gui_event *new_event = gui_event_new();
    if (!new_event)
        return NULL;

    // We have the new event copy over the old one clone it
    memcpy(new_event, event, sizeof(*new_event));
    return new_event;
}

bool gui_event_condition_mouse_click(struct gui_event *event, struct gui_element *recv_element, va_list va_list)
{
    // Not mouse click dont deal with it
    if (event->type != GUI_EVENT_TYPE_ELEMENT_CLICKED)
    {
        return false;
    }

    int win_rel_x = event->data.click.coords.x;
    int win_rel_y = event->data.click.coords.y;
    return gui_element_exists_in_position(recv_element, win_rel_x, win_rel_y);
}

int gui_event_push_event_element_unfocus(struct gui *gui, struct gui_element *element)
{
    if (gui != element->gui)
    {
        return -EINVARG;
    }

    return gui_event_push(gui, &(struct gui_event){.type = GUI_EVENT_TYPE_ELEMENT_UNFOCUSED, .data.element_unfocus.element = element}, NULL);
}
int gui_event_push_event_element_focus(struct gui *gui, struct gui_element *element)
{
    if (gui != element->gui)
    {
        return -EINVARG;
    }

    return gui_event_push(gui, &(struct gui_event){.type = GUI_EVENT_TYPE_ELEMENT_FOCUSED, .data.element_focus.element = element}, NULL);
}

bool gui_event_condition_element_match(struct gui_event *event, struct gui_element *recv_element, va_list va_list)
{
    struct gui_element *match_to_element = va_arg(va_list, struct gui_element *);
    if (!match_to_element)
        return false;

    if (match_to_element == recv_element)
        return true;

    return false;
}

int gui_event_push_event_keypress(struct gui *gui, int key, struct gui_element *element)
{
    return gui_event_push(gui, &(struct gui_event){.type = GUI_EVENT_TYPE_KEYSTROKE, .data.keystroke.key = key}, gui_event_condition_element_match);
}
int gui_event_push_event_mouse_click(struct gui *gui, int window_rel_click_x, int window_rel_click_y, int type)
{

    return gui_event_push(gui, &(struct gui_event){.type = GUI_EVENT_TYPE_ELEMENT_CLICKED, .data.click.coords = {.x = window_rel_click_x, .y = window_rel_click_y}, .data.click.type = type}, gui_event_condition_mouse_click);
}

static void gui_event_push_to_property(struct gui_event *event, struct gui_element_property *property)
{
    struct gui_element *element = event->element.ptr;
    if (!element || !property)
    {
        return;
    }

    if (property->listeners.event)
    {
        property->listeners.event(element, event);
    }

    switch (event->type)
    {
    case GUI_EVENT_TYPE_ELEMENT_CLICKED:
        if (property->listeners.click)
        {
            property->listeners.click(element, event->data.click.coords.x, event->data.click.coords.y);
        }
        break;
    default:
        break;
    }
}

static void gui_event_push_to_properties(struct gui_event *event)
{
    struct gui_element *element = event->element.ptr;
    if (!element)
    {
        return;
    }

    struct vector *property_vector = element->properties;
    size_t total_properties = vector_count(property_vector);
    for (size_t i = 0; i < total_properties; i++)
    {
        struct gui_element_property *property = NULL;
        vector_at(property_vector, i, &property, sizeof(property));
        if (property)
        {
            gui_event_push_to_property(event, property);
        }
    }
}

int gui_event_push(struct gui *gui, struct gui_event *event, GUI_EVENT_CONDITION_CHECK chck_func, ...)
{
    va_list ap;
    va_start(ap, chck_func);

    // Let's clone the event this will allow people
    // to pass events on the stack much simpler for them
    // with initializers too i.e {.type=ABC,.data.click=1}

    if (event->element.ptr)
    {
        // User should not provide an element for this high level abstraction
        // user should use gui_element_event_push
        return -EINVARG;
    }


    // Let's grab the ID from the element pointer and set the ID In the struct
    // regarldes sof what the user originally provided
    // heap_event->element.id = heap_event->element.ptr.id;
    // TODO IMPLEMENT AN ID FOR GUI ELEMENS

    // Now we have the clone lets call each childs handler
    // it will be responsible for its own children

    size_t total_root_children = vector_count(gui->elements);
    for (size_t i = 0; i < total_root_children; i++)
    {
        struct gui_element *child_elem = NULL;
        vector_at(gui->elements, i, &child_elem, sizeof(child_elem));
        if (child_elem && child_elem->functions.event_handler)
        {
            // Let's make sure that we are expected to push to this gui element
            // no check function then we send to all children.
            bool send_to_child = chck_func ? chck_func(event, child_elem, ap) : true;
            if (send_to_child)
            {
                gui_element_event_clone_push(child_elem, event);
            }
        }
    }

    va_end(ap);
    return 0;
}

bool gui_event_should_continue_propergating_childern(GUI_EVENT_HANDLER_RESPONSE res)
{
    return res == GUI_EVENT_HANDLER_RESPONSE_IGNORED ||
           res == GUI_EVENT_HANDLER_RESPONSE_ERROR ||
           res == GUI_EVENT_HANDLER_RESPONSE_PROCESSED_CONTINUE_WITH_CHILDREN;
}

void gui_event_propergate_through_children(struct gui_event *gui_event)
{
    struct gui_element *element = gui_event->element.ptr;
    size_t total_children = vector_count(element->children);
    for (size_t i = 0; i < total_children; i++)
    {
        struct gui_element *child = NULL;
        vector_at(element->children, i, &child, sizeof(child));
        if (child && child->functions.event_handler)
        {
            gui_element_event_clone_push(child, gui_event);
        }
    }
}

void gui_element_call_event_handlers(struct gui_element *element, struct gui_event *event)
{
    GUI_EVENT_HANDLER_FUNCTION func = element->functions.event_handler;
    if (func)
    {
        int res = func(event);
        if (gui_event_should_continue_propergating_childern(res))
        {
            // We are expected to propergate the event
            // down the children tree
            gui_event_propergate_through_children(event);
        }
    }
}

void gui_element_events_process_event(struct gui_event* event)
{
    struct gui_element* element = event->element.ptr;

    // Properties may want to capture the incoming event early.
    gui_event_push_to_properties(event);

    gui_element_call_event_handlers(element, event);
}
void gui_element_events_process(struct gui* gui)
{
    struct gui_event* event = NULL;
    while (vector_count(gui->gui_events) > 0)
    {
        // Pull the newest queued event off the queue
        vector_back(gui->gui_events, &event, sizeof(event));
        vector_pop(gui->gui_events);
        if (!event)
        {
            continue;
        }

        gui_element_events_process_event(event);
        gui_event_free(event);
    }
}
void gui_element_event_push(struct gui_element *element, struct gui_event *event)
{

    struct gui_event *heap_event = gui_event_clone(event);
    if (!heap_event)
        return ;

    heap_event->gui = element->gui;
    // Let's update the element to be the one we are pushing
    heap_event->element.ptr = element;
    heap_event->element.id = element->id;

    vector_push(element->gui->gui_events, &heap_event);
}

#warning "DEPRECATED DONT USE THIS FUCNTION"
int gui_element_event_clone_push(struct gui_element *element, struct gui_event *event)
{   
    gui_element_event_push(element, event);
    return 0;
}
