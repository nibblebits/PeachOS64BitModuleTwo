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

#ifndef USERLAND_GUI_LISTAREA_H
#define USERLAND_GUI_LISTAREA_H
#include "vector.h"
#include "../graphics.h"

#define LISTAREA_MAX_VALUE_SIZE 256
#define LISTAREA_BORDER_WIDTH_PX 3

struct gui;

struct listarea_list_element
{
    char text[LISTAREA_MAX_VALUE_SIZE];
    // Private data for the listarea element
    void* private;
};

struct font;
struct listarea_private_data
{
    // Vector of elements struct listarea_list_element*
    struct vector* elements;

    // The index in the vector that is selected.
    int selected_index;

    struct framebuffer_pixel selected_bg_color;
    struct framebuffer_pixel border_color;
    struct framebuffer_pixel font_color;

    struct font* font;
};

struct gui_element *gui_element_listarea_create(struct gui *gui, struct gui_element *parent, int x, int y, int width, int height, int id);
int gui_element_listarea_list_add(struct gui_element* gui_element, const char* title, void* private);
void gui_element_listarea_list_remove(struct gui_element* gui_element, int index);
size_t gui_element_listarea_height_per_element(struct gui_element* element);
struct framebuffer_pixel gui_element_listarea_selected_color(struct gui_element* element);
struct framebuffer_pixel gui_element_listarea_background_color(struct gui_element* element);
struct listarea_list_element* gui_element_listarea_list_element(struct gui_element* element, int index);
size_t gui_element_listarea_total_elements(struct gui_element* listarea_element);
struct font* gui_element_listarea_font(struct gui_element* listarea_element);
int gui_element_listarea_select(struct gui_element* listarea_element, int index);
int gui_element_listarea_selected_index(struct gui_element* listarea_element);

#endif