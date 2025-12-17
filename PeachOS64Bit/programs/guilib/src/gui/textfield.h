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

#ifndef USERLAND_TEXTFIELD_H
#define USERLAND_TEXTFIELD_H
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "graphics.h"
struct gui;
struct gui_element;
struct font;

#define TEXTFIELD_DEFAULT_BUFFER_SIZE 128

enum
{
    GUI_TEXTFIELD_VERTICAL_ALIGNMENT_TOP = 0,
    GUI_TEXTFIELD_VERTICAL_ALIGNMENT_CENTER = 1
};

enum
{
    GUI_TEXTFIELD_IS_MULTILINE_FLAG = 0b00000001,
    GUI_TEXTFIELD_IS_READ_ONLY_FLAG = 0b00000010
};

typedef int GUI_TEXTFIELD_ALIGNMENT;
// Ascii table backspace character code
#define GUI_KEY_BACKSPACE 0x08


struct textfield_private_data
{
    int flags;
    struct textfield_text_alignment
    {
        GUI_TEXTFIELD_ALIGNMENT vertical;
    } text_alignment;

    struct textfield_text
    {
        // The text currently entered into the textfield
        char *text;
        // Note: if this is zero then the text will be reallocated
        // once the bounds is exceeded
        size_t max_allowed_len;
        // Current memory allocation of the text
        // if current_len >= current_allocated_len reallocation required
        size_t current_allocated_len;
        size_t current_len;

        // The colour for the textfield text
        struct framebuffer_pixel color; //American english better for universally.
    } text;

    // Next index where next char will be put
    int index;

    // The font of the text to use.
    struct font *font;
};

struct gui_element* gui_element_textfield_create(struct gui* gui, struct gui_element* parent, int x, int y, int width, int height, int id);

/**
 * Set the text alignment of the text
 */
void gui_element_textfield_text_alignment_set(struct gui_element *element, GUI_TEXTFIELD_ALIGNMENT alignment);
void gui_element_textfield_flags_set(struct gui_element* element, int flags);
void gui_element_textfield_color_set(struct gui_element* element, int red, int green, int blue);
void gui_element_textfield_read_only_set(struct gui_element* element, bool read_only);
bool gui_element_textfield_read_only(struct gui_element* element);
void gui_element_textfield_cursor_set(struct gui_element* element, int index);
void gui_element_textfield_clear(struct gui_element* element);

/*
 * Gets the current  text string value of the given textfield entity
 */
const char *gui_element_textfield_text(struct gui_element *element);

/**
 * Sets the text of the textfield entity
 */
void gui_element_textfield_text_set(struct gui_element *element, const char *text);

/**
 * Writes the single character to the end of the string of the
 * text field entity.
 */
void gui_element_textfield_put_char(struct gui_element *element, char c);

/**
 * Makes the textfield backspace the text erasing the most recent character.
 */
void gui_element_textfield_backspace(struct gui_element *element);
#endif
