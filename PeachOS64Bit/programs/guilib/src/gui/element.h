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

#ifndef USERLAND_GUI_ELEMENT_H
#define USERLAND_GUI_ELEMENT_H
#include "vector.h"

// WARNING: POSSIBLE FORWARD DECLARATION
// CIRCULAR ISSUES, KEEP NOTE
#include "event.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

struct gui;
struct gui_element;
struct framebuffer_pixel;
struct graphics;


typedef void(*GUI_ELEMENT_DRAW)(struct gui_element* elem);
typedef void(*GUI_ELEMENT_FREE)(struct gui_element* elem);

enum
{
    GUI_ELEMENT_REDRAW_REQUIRED = 0b00000001,
    GUI_ELEMENT_IS_ROOT_ELEMENT = 0b00000010,
    // If the flag is set
    // the pixels will be allowed to be larger than the actual
    // element dimensions
    // and a scrollable section shall exist.
    GUI_ELEMENT_IS_SCROLLABLE   = 0b00000100,
};

struct gui_element
{
    // Id of the element
    int id;

    // Flags
    int flags;

    // Coordinates are absolute but relative to the window body
    int x;
    int y;

    // relative coordinates from the parent gui_element
    int rel_x;
    int rel_y;

    // relative coordinates from the oldest grandparent, i.e the root
    // element that is registered with the gui struct.
    int rel_root_x;
    int rel_root_y;

    // Width and height of such element.
    int width;
    int height;

    // Max width/height from a kernels prespective
    // no element may everflow or will result in a seg fault
    // and program temrination.
    int real_width;
    int real_height;

    struct 
    {
        // Draw function for this gui element
        GUI_ELEMENT_DRAW draw;
        GUI_ELEMENT_FREE free;
        // For handling events related to this element
        GUI_EVENT_HANDLER_FUNCTION event_handler;
    } functions;
    
    // The graphics entity for this element
    struct graphics* graphics;

    // A pointer to the pixels of this graphical entity
    struct framebuffer_pixel* pixels;

    struct gui_element* parent;

    // Elements can have multiple children
    // vector of struct gui_element*
    struct vector* children;


    // vector of struct gui_element_property*
    struct vector* properties;

    struct gui* gui;

    // Private data belonging to the gui element
    void* private;
};

bool gui_element_should_redraw(struct gui_element* element);
void gui_element_mark_for_redraw(struct gui_element* element);
void gui_element_redrawn(struct gui_element* element);

void gui_element_draw(struct gui_element* element);
struct gui_element* gui_element_parent(struct gui_element* element);

/**
 * Gets the top most element given a nested element of children
 * if you are top most element you are returned.
 */
struct gui_element* gui_element_root(struct gui_element* element);


struct gui_element *gui_element_create(struct gui *gui, struct gui_element *parent, int x, int y, int width, int height, GUI_ELEMENT_DRAW draw_function, GUI_ELEMENT_FREE free_function, int id);

bool gui_element_bounds_check(struct gui_element* element, int x, int y);
void gui_element_draw_rect(struct gui_element* element, int x, int y, int width, int height, struct framebuffer_pixel* pixel_color);
void gui_element_draw_pixel(struct gui_element *element, int x, int y, struct framebuffer_pixel *pixel);
int gui_element_pixel_get(struct gui_element* element, int x, int y, struct framebuffer_pixel* pixel_out);

/**
 * Should be called only from within a draw function.
 */
void gui_element_draw_border(struct gui_element *gui_element, int border_width, struct framebuffer_pixel* color);

/**
 * NOTE WHEN ADDED TO THE GUI THE OWNERSHIP IS WITH THE GUI
 * DO NOT DELETE AFTER THAT POINT
 */
void gui_element_free(struct gui_element* element);

void gui_element_private_set(struct gui_element* element, void* private);


/**
 * Poorly named function, this function will check if a gui element
 * exists in the given position relative to the window position.
 * 
 * NOTE; DO better think of a better name this is an awful name.
 * \return Returns true fi the element exists at the given position
 * relative to the window coordinates.
 */
bool gui_element_exists_in_position(struct gui_element* element, int x, int y);
void gui_element_event_handler_set(struct gui_element* element, GUI_EVENT_HANDLER_FUNCTION handler_func);

// Public because people might want to call the parent handler.
// No oop in c
GUI_EVENT_HANDLER_RESPONSE gui_element_event_handler(struct gui_event* gui_event);



#endif
