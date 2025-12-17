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

#ifndef USERSPACE_GUI_EVENT_H
#define USERSPACE_GUI_EVENT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

struct gui_event;
enum
{
    GUI_EVENT_TYPE_ELEMENT_CLICKED,
    GUI_EVENT_TYPE_ELEMENT_FOCUSED,
    GUI_EVENT_TYPE_ELEMENT_UNFOCUSED,
    GUI_EVENT_TYPE_KEYSTROKE,
    GUI_EVENT_EXIT,
    // Consider moving to listarea.h as its more custom than this.
    // I'll need to think on the best design stratergy.
    GUI_EVENT_TYPE_LISTAREA_ITEM_SELECTED,

};

enum
{
    GUI_LEFT_CLICK,
    GUI_RIGHT_CLICK,
    GUI_MIDDLE_CLICK
};

enum
{
    // Returned by event handlers that didnt bother to handle
    // this event o rdidnt need too.
    GUI_EVENT_HANDLER_RESPONSE_ERROR = -1,
    GUI_EVENT_HANDLER_RESPONSE_IGNORED = 0,
    // Returned when the element has handled the event
    // but wishes all other children to be passed the event
    GUI_EVENT_HANDLER_RESPONSE_PROCESSED_CONTINUE_WITH_CHILDREN,
    // Returned when the element has handled the event
    // and does not want this event passed to children
    GUI_EVENT_HANDLER_RESPONSE_PROCESSED_AND_FINISHED
};

struct gui;
struct gui_element;
struct gui_event;

typedef int GUI_EVENT_HANDLER_RESPONSE;

typedef GUI_EVENT_HANDLER_RESPONSE (*GUI_EVENT_HANDLER_FUNCTION)(struct gui_event* gui_event);
typedef bool (*GUI_EVENT_CONDITION_CHECK)(struct gui_event* gui_event, struct gui_element* recv_element, va_list va_list);


struct gui_event
{
    // Pointer to the GUI instance.
    struct gui* gui;

    // gui-type..
    int type;
    struct
    {
        int id;
        struct gui_element* ptr;
    } element;
    union 
    {
        // The ID of the element that an action has taken place on
  
        struct gui_event_element_click
        {
            // i.e left, right, middle mouse button click
            int type;

            // Absolute window coordinates
            // that the click took place.
            // or relative coordinates on an element
            // if an element is set.
            struct
            {
                int x;
                int y;
            } coords;
        } click;

        struct gui_event_element_focus
        {
            // The element that has been focused.
            struct gui_element* element;
        } element_focus;

        struct gui_event_element_unfocus
        {
            // The element that was unfocused.
            struct gui_element* element;
        } element_unfocus;

        struct gui_event_element_keystroke
        {
            int key;
        } keystroke;

        // This isnt abstract enough consider a way to extend the event.
        struct gui_event_listarea_item_selected
        {
            int selected_index;
        } listarea_item_selected;
    } data;
    
};

struct gui_event* gui_event_new();
void gui_event_free(struct gui_event* gui_event);
struct gui_event* gui_event_clone(struct gui_event* event);
int gui_event_push_event_element_focus(struct gui* gui, struct gui_element* element);
int gui_event_push_event_element_unfocus(struct gui *gui, struct gui_element *element);

int gui_event_push_event_mouse_click(struct gui* gui, int window_rel_click_x, int window_rel_click_y, int type);
int gui_event_push(struct gui* gui, struct gui_event* event, GUI_EVENT_CONDITION_CHECK chck_func, ...);
void gui_event_propergate_through_children(struct gui_event* gui_event);
bool gui_event_should_continue_propergating_childern(GUI_EVENT_HANDLER_RESPONSE res);
void gui_element_event_push(struct gui_element* element, struct gui_event* event);
int gui_element_event_clone_push(struct gui_element* element, struct gui_event* event);

void gui_element_events_process(struct gui* gui);

#endif
