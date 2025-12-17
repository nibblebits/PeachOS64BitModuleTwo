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

#ifndef USERLAND_ELEMENT_PLANE_H
#define USERLAND_ELEMENT_PLANE_H
#include "../graphics.h"
struct gui;
struct gui_element;

#define GUI_PLANE_DEFAULT_FOCUSED_BORDER_WIDTH 5

enum
{
    USERLAND_PLANE_FLAG_DRAW_BORDER_ON_FOCUS = 0b00000001,
    USERLAND_PLANE_FLAG_DRAW_STANDARD_BORDER = 0b00000010,

    // When set if you click the plane
    // it will darken for a moment
    // then return to normal
    USERLAND_PLANE_FLAG_DARKEN_ON_CLICK      = 0b00000100
};

// Can be a parent to other objects that
// want to share properties
// who said OOP couldnt happen in C
struct userland_element_plane_private
{
    // Flags for this plane element.
    int flags;

    // Background of the plane
    struct
    {
        int red;
        int blue;
        int green;
    } bg;


    // The border color for when its focused
    struct
    {
        struct framebuffer_pixel color;
        // Width in pixels of the border
        int width;
    } focused_border;


    // Normal non-focused border
    struct 
    {
        struct framebuffer_pixel color;
        int width;
    } border;
    

    
    // Pointer to our element
    struct gui_element* element;
};

struct framebuffer_pixel gui_element_plane_bg_color_get(struct gui_element* gui_element);

struct gui_element *gui_element_plane_create(struct gui *gui, struct gui_element *parent, int x, int y, int width, int height, int id);
void gui_element_plane_set_focused_border_color(struct gui_element* gui_element, int red, int green, int blue);
void gui_element_plane_bg_color_set(struct gui_element* gui_element, int red, int green, int blue);
void gui_element_plane_flag_set(struct gui_element* gui_element, int flag);
void gui_element_plane_focused_border_width_set(struct gui_element* gui_element, int pixel_width);
void gui_element_plane_set_border_color(struct gui_element* gui_element, int red, int green, int blue);
void gui_element_plane_set_border_width(struct gui_element* gui_element, int width);
bool gui_element_plane_should_draw_border(struct gui_element* element);

#endif