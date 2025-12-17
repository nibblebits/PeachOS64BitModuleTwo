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

#include "button.h"
#include "element.h"
#include "plane.h"
#include "stdlib.h"
#include "memory.h"
#include "string.h"
#include "font.h"
#include "stdio.h"
GUI_EVENT_HANDLER_RESPONSE gui_element_button_event_handler(struct gui_event *gui_event)
{
    // Blank handler.

    // Mark as finished so our children dont get the response.
    return GUI_EVENT_HANDLER_RESPONSE_PROCESSED_AND_FINISHED;
}

void gui_element_button_draw(struct gui_element *button_element)
{
    struct gui_button_element_private *private = (struct gui_button_element_private *)button_element->private;

    int x_pos = button_element->width / 2;
    int y_pos = button_element->height / 2;

    struct font *font = private->font;
    if (!font)
        return;

    // We must take into account the text length
    // and the font size to ensure w ehave an accurate centering
    int total_characters = strlen(private->button_text);
    int text_pixel_size_width = font->bits_width_per_character * total_characters;
    int text_pixel_half_width = text_pixel_size_width / 2;
    // We have copmputed the size of the pixel data for
    // this given sentance
    // lets adjust the center position
    // half width as we want this centered horiziontally
    x_pos -= text_pixel_half_width;

    if (x_pos < 0)
    {
        x_pos = 0;
    }

    struct framebuffer_pixel black = {0};
    font_draw_text(button_element->graphics, NULL, x_pos, y_pos, private->button_text, black);
}

void gui_element_button_free(struct gui_element *button_element)
{
    if (button_element->private)
    {
        // free the private data
        free(button_element->private);
    }
}
struct gui_element *gui_element_button_create(struct gui *gui, struct gui_element *parent, int x, int y, int width, int height, const char *text, int id)
{
    // Create the plane we will be its child
    struct gui_element *plane_element = gui_element_plane_create(gui, parent, x, y, width, height, id);
    if (!plane_element)
    {
        // Out of memory.
        return NULL;
    }

    // let's set the plane background
    // NOTE: THIS CALL DOES NOT CAUSE THE BUG
    // BUT LATER ON THE DRAWING OF THOSE PIXELS OVERWRITES THE MEMORY
    gui_element_plane_bg_color_set(plane_element, 0xAA, 0xAA, 0xAA);

    // We want it look like you actually clicked the button
    // this flag will help achieve that. The button shall darken
    // for a moment once pressed.
    gui_element_plane_flag_set(plane_element, USERLAND_PLANE_FLAG_DARKEN_ON_CLICK);

    // create a new ele,ent for out button
    // 0:X 0:Y because its relative coordinates to the parent.
    struct gui_element *btn_element = gui_element_create(gui, plane_element, 0, 0, width, height, gui_element_button_draw, gui_element_button_free, id);
    if (!btn_element)
    {
        // out of memory. FIX MEMORY LEAK ABOVE LATER..
        return NULL;
    }

    struct gui_button_element_private *button_element_private_data = calloc(1, sizeof(struct gui_button_element_private));
    if (!button_element_private_data)
    {
        // Out of memory, FIX MEMORY LEAK LATER..
        return NULL;
    }

    // System font by default for all buttons.
    button_element_private_data->font = font_get_system_font();

    // Copy the text to the button capped at its max size
    if (text)
    {
        strncpy(button_element_private_data->button_text, text, sizeof(button_element_private_data->button_text));
    }
    // Set the private data.
    gui_element_private_set(btn_element, button_element_private_data);

    // Setup event handler for the button
    gui_element_event_handler_set(btn_element, gui_element_button_event_handler);

    // Button now constructed, composes of a plane and button element
    return btn_element;
}