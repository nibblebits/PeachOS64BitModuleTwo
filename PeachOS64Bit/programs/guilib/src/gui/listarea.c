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

#include "listarea.h"
#include "plane.h"
#include "status.h"
#include "element.h"
#include "stdlib.h"
#include "stdio.h"
#include "memory.h"
#include "string.h"
#include "font.h"

size_t gui_element_listarea_height_per_element(struct gui_element* element)
{
    struct font* font = gui_element_listarea_font(element);
    return (3 + font->bits_height_per_character);
}

struct framebuffer_pixel gui_element_listarea_selected_color(struct gui_element* element)
{
    // for now a fixed color
    struct framebuffer_pixel color = {.red=0xf3, .green=0x00, .blue=0x00};
    return color;
}

struct framebuffer_pixel gui_element_listarea_background_color(struct gui_element* element)
{
    // Fixed for now will add adjustable later..
    struct framebuffer_pixel color = {0xA0, 0xA0, 0xA0};
    return color;
}

void gui_element_listarea_draw(struct gui_element* element)
{
    // We shall loop through every element and draw it
    struct listarea_private_data* private_data = element->private;
    size_t total_elements = vector_count(private_data->elements);
    int x_rel_pos = 0;
    int y_rel_pos = 0;
    struct font* font = gui_element_listarea_font(element);
    size_t size_per_element = gui_element_listarea_height_per_element(element);
    struct framebuffer_pixel selected_color = gui_element_listarea_selected_color(element);
    struct framebuffer_pixel bg_color = gui_element_listarea_background_color(element);
    // Since this is a fresh redraw we must first redraw the whole listarea as solid color
    graphics_draw_rect(element->graphics, 0, 0, element->width, element->height, bg_color);
    for(size_t i = 0 ;i < total_elements; i++)
    {
        struct listarea_list_element* list_element = NULL;
        vector_at(private_data->elements,i,  &list_element, sizeof(list_element));
        if (list_element)
        {
            // Is this element selected if so we need to draw it as so
            if (i == private_data->selected_index)
            {
                // Yep, then lets draw the border
                graphics_draw_rect(element->graphics, 0, i * (int) size_per_element, element->width, size_per_element, selected_color);
            }
            // We have a valid element lets draw the thing
            font_draw_text(element->graphics, font, x_rel_pos, y_rel_pos, list_element->text, private_data->font_color);
            // We need to now draw a border
            // and make room for the text height
            y_rel_pos += font->bits_height_per_character;
            graphics_draw_rect(element->graphics, x_rel_pos, y_rel_pos, element->width, LISTAREA_BORDER_WIDTH_PX, private_data->border_color);
            // Now we have drawn the border letsm ake room for the next text element
            y_rel_pos += LISTAREA_BORDER_WIDTH_PX;
        }
    }
}


void gui_element_listarea_list_element_free(struct listarea_list_element* element)
{
    free(element);
}

void gui_element_listarea_free_vector_elements(struct gui_element* element)
{
    struct listarea_private_data* private_data = element->private;
    size_t total_elements = vector_count(private_data->elements);
    for(size_t i = 0; i < total_elements; i++)
    {
        struct listarea_list_element* element = NULL;
        vector_at(private_data->elements, i, &element, sizeof(element));
        if (element)
        {
            gui_element_listarea_list_element_free(element);
        }
    }
}
void gui_element_listarea_free(struct gui_element* element)
{
    struct listarea_private_data* private_data = element->private;
    // Free all the vector elements.
    gui_element_listarea_free_vector_elements(element);
    vector_free(private_data->elements);

    
}

struct listarea_list_element* gui_element_listarea_list_element(struct gui_element* element, int index)
{
    struct listarea_list_element* list_element = NULL;
    struct listarea_private_data* private_data = element->private;
    if (index >= vector_count(private_data->elements))
    {
        return NULL;
    }

    // Read the pointer now.
    vector_at(private_data->elements, index, &list_element, sizeof(list_element));
    return list_element;

}

void gui_element_listarea_list_remove(struct gui_element* gui_element, int index)
{
    struct listarea_private_data* private_data = gui_element->private;
    struct listarea_list_element* list_element = gui_element_listarea_list_element(gui_element, index);
    if (!list_element)
    {
        // Invalid element
        return;
    }

    gui_element_listarea_list_element_free(list_element);
    // We have a valid index pop that element
    vector_pop_element(private_data->elements, list_element, sizeof(list_element));
}

int gui_element_listarea_list_add(struct gui_element* gui_element, const char* title, void* private)
{
    int res = 0;
    struct listarea_private_data* private_data = gui_element->private;

    struct listarea_list_element* list_element = calloc(1, sizeof(struct listarea_list_element));
    if (!list_element)
    {
        res = -ENOMEM;
        goto out;
    }

    strncpy(list_element->text, title, sizeof(list_element->text));
    list_element->private = private;
    res = vector_push(private_data->elements, &list_element);
out:
    return res;
}   

size_t gui_element_listarea_total_elements(struct gui_element* listarea_element)
{
    struct listarea_private_data* private_data = listarea_element->private;
    return vector_count(private_data->elements);
}

struct font* gui_element_listarea_font(struct gui_element* listarea_element)
{
    struct listarea_private_data* private_data = listarea_element->private;
    if (private_data->font)
    {
        return private_data->font;
    }

    return font_get_system_font();
}

int gui_element_listarea_select(struct gui_element* listarea_element, int index)
{
    struct listarea_private_data* private_data = listarea_element->private;
    size_t total_elements = gui_element_listarea_total_elements(listarea_element);
    if (index >= total_elements)
    {
        return -EINVARG;
    }
    private_data->selected_index = index;
    gui_element_mark_for_redraw(listarea_element);

    // Select shall invoke the select event handler
    struct gui_event event = {0};
    event.type = GUI_EVENT_TYPE_LISTAREA_ITEM_SELECTED;
    event.data.listarea_item_selected.selected_index = index;
    gui_element_event_clone_push(listarea_element, &event);
    return 0;
}

int gui_element_listarea_selected_index(struct gui_element* listarea_element)
{
    struct listarea_private_data* private_data = listarea_element->private;
    return private_data->selected_index;
}


void gui_element_listarea_event_handler_click(struct gui_event* gui_event)
{
    struct gui_element* listarea_element = gui_event->element.ptr;
    int y = gui_event->data.click.coords.y;
    struct font* font = gui_element_listarea_font(listarea_element);

    // We need to determine which element was clicked, we know how to do this
    // because we understand the size of each element in height.
    // 3 PIXELS for border + height per characater, this is the height of each possible element
    size_t total_elements = gui_element_listarea_total_elements(listarea_element);
    int calculated_selected_index = y / (font->bits_height_per_character + 3);
    if (calculated_selected_index >= total_elements)
    {
        // Clicked out of bounds
        return;
    }


    // We have anew index clicked
    // lets selected this element
    gui_element_listarea_select(listarea_element, calculated_selected_index);
    
    
}
GUI_EVENT_HANDLER_RESPONSE gui_element_listarea_event_handler(struct gui_event* gui_event)
{
    switch(gui_event->type)
    {
        case GUI_EVENT_TYPE_ELEMENT_CLICKED:
            gui_element_listarea_event_handler_click(gui_event);
        break;
    }
    return GUI_EVENT_HANDLER_RESPONSE_PROCESSED_CONTINUE_WITH_CHILDREN;
}

struct gui_element *gui_element_listarea_create(struct gui *gui, struct gui_element *parent, int x, int y, int width, int height, int id)
{
    int res = 0;
    struct gui_element* element = NULL;
    struct listarea_private_data* private_data = NULL;
    // We need a solid plane for the background
    // of this listarea
    struct gui_element* plane = gui_element_plane_create(gui, parent, x, y, width, height, id);
    if (!plane)
    {
        return NULL;
    }

    element = gui_element_create(gui, plane, 0, 0, width, height, gui_element_listarea_draw, gui_element_listarea_free, id);
    if (!element)
    {
        res = -ENOMEM;
        goto out;
    }

    private_data = calloc(1, sizeof(struct listarea_private_data));
    if(!private_data)
    {
        res = -ENOMEM;
        goto out;
    }

    private_data->elements = vector_new(sizeof(struct listarea_list_element*), 4, 0);
    if (!private_data->elements)
    {
        res = -ENOMEM;
        goto out;
    }

    // Ranodm colors until i can make the functions that set them.
    private_data->font_color.blue = 0xff;
    private_data->selected_index = -1;
    private_data->selected_bg_color.red = 0xff;
    gui_element_private_set(element, private_data);

    // Default background colour, will make it changable later..
    gui_element_plane_bg_color_set(plane, 0xAA, 0xAA, 0xAA);
    
    //E vent handler for this lsitarea gui element
    gui_element_event_handler_set(element, gui_element_listarea_event_handler);


out:
    if (res < 0)
    {
        if (element)
        {
            gui_element_free(element);
        }
    }
    return element;
}