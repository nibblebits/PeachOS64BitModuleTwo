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

#include "textfield.h"
#include "element.h"
#include "plane.h"
#include "gui.h"
#include "font.h"
#include "status.h"
#include "memory.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
int gui_element_textfield_realloc_text(struct gui_element *element, size_t new_size);

/**
 * TODO: MANY SHARED PROPERTIES BETWEEN BUTTON AND TEXTFIELD
 * MAKE A PARENT INSTEAD... TODO
 */

void gui_element_textfield_put_char(struct gui_element *element, char c)
{
    struct textfield_private_data *private_data = element->private;
    // Have we already reached the maximum length allowed in this textfield
    // if so we will refuse the putchar

    // -1 as indexes start at zero, firs tcharacter always at zero
    if (private_data->index >= private_data->text.max_allowed_len - 1)
    {
        // Put char refused we reached the maximum allowed length
        return;
    }
    if (private_data->index >= private_data->text.current_allocated_len - 1)
    {
        // We cannot allow an overflow.
        // lets resize the buffer if we can
        size_t new_size = private_data->text.current_allocated_len + TEXTFIELD_DEFAULT_BUFFER_SIZE;
        int res = gui_element_textfield_realloc_text(element, new_size);
        if (res < 0)
        {
            // Realloc failed
            return;
        }
    }
    private_data->text.text[private_data->index] = c;
    private_data->index++;
    if (private_data->index >= private_data->text.current_len - 1)
    {
        private_data->text.current_len = private_data->index + 1;
    }

    // we must redraw this next cycle.
    gui_element_mark_for_redraw(element);
}

void gui_element_textfield_clear(struct gui_element *element)
{
    struct textfield_private_data *private_data = element->private;
    private_data->index = 0;
    private_data->text.current_len = 0;
    memset(private_data->text.text, 0x00, private_data->text.current_allocated_len);
}
void gui_element_textfield_cursor_set(struct gui_element *element, int index)
{
    struct textfield_private_data *private_data = element->private;
    private_data->index = index;
}
void gui_element_textfield_text_set(struct gui_element *element, const char *text)
{
    // Clear the old text.
    gui_element_textfield_clear(element);
    const char *ptr = text;
    while (*ptr)
    {
        gui_element_textfield_put_char(element, (char)*ptr);
        ptr++;
    }
}

const char *gui_element_textfield_text(struct gui_element *element)
{
    struct textfield_private_data *private_data = element->private;
    return private_data->text.text;
}

void gui_element_textfield_backspace(struct gui_element *element)
{
    struct textfield_private_data *private_data = element->private;
    if (private_data->index == 0)
    {
        // We cannot backspace further
        return;
    }

    private_data->text.text[private_data->index - 1] = 0x00;
    private_data->index--;

    gui_element_mark_for_redraw(element);
}

GUI_EVENT_HANDLER_RESPONSE gui_element_textfield_event_handler(struct gui_event *gui_event)
{
    if (gui_event->type != GUI_EVENT_TYPE_KEYSTROKE)
    {
        return GUI_EVENT_HANDLER_RESPONSE_IGNORED;
    }

    struct gui_element *element = gui_event->element.ptr;
    // If its a read only textfield we should do nothing in regards to keypress
    if (gui_element_textfield_read_only(element))
    {
        return GUI_EVENT_HANDLER_RESPONSE_IGNORED;
    }

    char key = (char)gui_event->data.keystroke.key;
    if (key == GUI_KEY_BACKSPACE)
    {
        gui_element_textfield_backspace(element);
        return GUI_EVENT_HANDLER_RESPONSE_PROCESSED_CONTINUE_WITH_CHILDREN;
    }

    // Not backspace, put the character.
    gui_element_textfield_put_char(element, key);
    return GUI_EVENT_HANDLER_RESPONSE_PROCESSED_CONTINUE_WITH_CHILDREN;
}
void gui_element_textfield_draw(struct gui_element *gui_element)
{
    struct textfield_private_data *private = gui_element->private;

    struct font *font = private->font;
    if (!font)
        return;

    int x_pos = gui_element->width / 2;

    // BY default the alignment is GUI_TEXTFIELD_VERTICAL_ALIGNMENT_TOP
    // with a y_pos of zero.
    int y_pos = 0;
    if (private->text_alignment.vertical == GUI_TEXTFIELD_VERTICAL_ALIGNMENT_CENTER)
    {
        y_pos = gui_element->height / 2;
    }

    // If its a read only textfield then we should draw the background slightly darker
    if (gui_element_textfield_read_only(gui_element))
    {
        struct framebuffer_pixel bg_color = {0};
        struct gui_element *plane_parent = gui_element->parent;
        bg_color = gui_element_plane_bg_color_get(plane_parent);
        // Now we have the actual colour of the plane parent
        // lets darken it slightly to give illusion of read only.
        // Not sure if this will work though we shall see.
        bg_color.red -= 10;
        bg_color.blue -= 10;
        bg_color.green -= 10;

        gui_element_draw_rect(gui_element, 0, 0, gui_element->width, gui_element->height, &bg_color);
    }

    // Draw the text of the text element.
    x_pos = 0;

    // OLD SYSTEM
    // int total_characters = private->text.current_len;

    // int text_pixel_size_width = font->bits_width_per_character * total_characters;
    // int text_pixel_half_width = text_pixel_size_width / 2;
    // // We have copmputed the size of the pixel data for
    // // this given sentance
    // // lets adjust the center position
    // // half width as we want this centered horiziontally
    // x_pos -= text_pixel_half_width;

    // if (x_pos < 0)
    // {
    //     x_pos = 0;
    // }

    // Use the colour the user wants.
    struct framebuffer_pixel colour = private->text.color;

    // Are we multiline?
    if (private->flags & GUI_TEXTFIELD_IS_MULTILINE_FLAG)
    {
        font_draw_text_wrap(gui_element->graphics, NULL, x_pos, y_pos, gui_element->width, gui_element->height, private->text.text, colour);
    }
    else
    {
        // Not multiline? great draw the text as seen.
        font_draw_text(gui_element->graphics, NULL, x_pos, y_pos, private->text.text, colour);
    }
}

void gui_element_textfield_free(struct gui_element *gui_element)
{
    // Free the private data
    free(gui_element->private);
}

bool gui_element_textfield_read_only(struct gui_element *element)
{
    struct textfield_private_data *private_data = element->private;
    return private_data->flags & GUI_TEXTFIELD_IS_READ_ONLY_FLAG;
}

void gui_element_textfield_read_only_set(struct gui_element *element, bool read_only)
{
    struct textfield_private_data *private_data = element->private;
    private_data->flags &= ~GUI_TEXTFIELD_IS_READ_ONLY_FLAG;
    if (read_only)
    {
        private_data->flags |= GUI_TEXTFIELD_IS_READ_ONLY_FLAG;
    }
    gui_element_mark_for_redraw(element);
}
void gui_element_textfield_text_vertical_alignment_set(struct gui_element *element, GUI_TEXTFIELD_ALIGNMENT alignment)
{
    struct textfield_private_data *private_data = element->private;
    private_data->text_alignment.vertical = alignment;
}

int gui_element_textfield_realloc_text(struct gui_element *element, size_t new_size)
{
    int res = 0;
    struct textfield_private_data *private_data = element->private;
    // We shall always alllow reallocations to huge sizes or smalelr ones
    // the check for exceeding max size will be done else where
    // this abstraction is cleaner.
    if (private_data->text.text == NULL)
    {
        private_data->text.text = calloc(1, new_size);
        // Current len i.e how many characters have been written so far.
        private_data->text.current_len = 0;
        private_data->text.current_allocated_len = new_size;
    }
    else if (private_data->text.current_allocated_len != new_size)
    {
        private_data->text.text = realloc(private_data->text.text, new_size);
        // If we allocated down then we will need to trim the rest of the data
        // and reposition the textfield
        private_data->text.current_allocated_len = new_size;
        if (private_data->text.current_len >= private_data->text.current_allocated_len)
        {
            private_data->text.current_len = private_data->text.current_allocated_len - 1;
            private_data->index = private_data->text.current_len - 1;
        }
    }

    // Redraw required after such action
    gui_element_mark_for_redraw(element);
out:

    return res;
}

void gui_element_text_field_max_characters_set(struct gui_element *element, size_t max_characters_allowed)
{
    struct textfield_private_data *private_data = element->private;
    private_data->text.max_allowed_len = max_characters_allowed;
}

void gui_element_textfield_color_set(struct gui_element *element, int red, int green, int blue)
{
    struct textfield_private_data *private_data = element->private;
    struct framebuffer_pixel color = {0};
    color.red = red;
    color.green = green;
    color.blue = blue;
    private_data->text.color = color;
}

void gui_element_textfield_flags_set(struct gui_element *element, int flags)
{
    struct textfield_private_data *private_data = element->private;
    private_data->flags = flags;

    // Maybe the way the text should be drawn has changed
    // now this flag is set.
    gui_element_mark_for_redraw(element);
}
struct gui_element *gui_element_textfield_create(struct gui *gui, struct gui_element *parent, int x, int y, int width, int height, int id)
{
    int res = 0;
    struct gui_element *element = NULL;
    struct textfield_private_data *private_data = NULL;
    struct gui_element *plane_bg_element = gui_element_plane_create(gui, NULL, x, y, width, height, id);
    if (!plane_bg_element)
    {
        res = -ENOMEM;
        goto out;
    }

    element = gui_element_create(gui, plane_bg_element, 0, 0, width, height, gui_element_textfield_draw, gui_element_textfield_free, id);
    if (!element)
    {
        res = -ENOMEM;
        goto out;
    }

    // Background color for the textfield, light gray color. #d3d3d3
    gui_element_plane_bg_color_set(plane_bg_element, 0xd3, 0xd3, 0xd3);

    private_data = calloc(1, sizeof(struct textfield_private_data));
    if (!private_data)
    {
        res = -ENOMEM;
        goto out;
    }

    // We shall use the system font for the textfield.
    private_data->font = font_get_system_font();

    // NOTE: on error as we've set the private data gui_element_free
    // shall delete this private data so dont worry about it
    gui_element_private_set(element, private_data);

    // We want the text aligned in the center by default
    // gui_element_textfield_text_vertical_alignment_set(element, GUI_TEXTFIELD_VERTICAL_ALIGNMENT_CENTER);
    // Is the height too low? if so align text to top so it doesnt look stupid
    // If the height is below the height of character multiplied by 2
    // then we shall never align vertically centered it could be cut off.

    // Make these variables smaller at some point..
    size_t max_acceptable_font_height_for_center_alignment = private_data->font->bits_height_per_character * 2;
    if (height < max_acceptable_font_height_for_center_alignment)
    {
        // We need vertical alignment at the top
        gui_element_textfield_text_vertical_alignment_set(element, GUI_TEXTFIELD_VERTICAL_ALIGNMENT_TOP);
    }

    // We will set the max characters to the default size
    // this is changable at any time
    gui_element_text_field_max_characters_set(element, TEXTFIELD_DEFAULT_BUFFER_SIZE);

    // Given the max characters has been set we now must resize/create the text
    // since its currently null, it shall be reallocated
    gui_element_textfield_realloc_text(element, TEXTFIELD_DEFAULT_BUFFER_SIZE);

    // Set the event handler to receive keystrokes.
    gui_element_event_handler_set(element, gui_element_textfield_event_handler);
out:
    if (res < 0)
    {
        if (plane_bg_element)
        {
            gui_element_free(plane_bg_element);
        }
        if (element)
        {
            gui_element_free(element);
        }
    }
    return element;
}