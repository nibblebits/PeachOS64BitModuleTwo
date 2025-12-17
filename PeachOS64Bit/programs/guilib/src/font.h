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

#ifndef USERLAND_GRAPHICS_FONT_H
#define USERLAND_GRAPHICS_FONT_H

// FINISH THIS LATER..
#include <stdint.h>
#include <stddef.h>
#include "graphics.h"

// 32 is space in ascii table
// the font character data will begin there.

// We subtract from the given character ascii code to draw by the below amount
// this ensures it matches where the character will be in the character array.
// essentially subtracting it from the charactr ascii code gives the index in the array
// where the starting pixel data is for that character.
#define FONT_IMAGE_DRAW_SUBTRACT_FROM_INDEX 32
// 9x16 per character.
#define FONT_IMAGE_CHARACTER_WIDTH_PIXEL_SIZE 9
#define FONT_IMAGE_CHARACTER_HEIGHT_PIXEL_SIZE 16
#define FONT_IMAGE_CHARACTER_Y_OFFSET 4

struct font
{
    // Follows the ASCII table from 0 to max 255
    // character should start from SPACE on ascii table
    size_t character_count;
    // Each bit represents one pixel of a given character, the character can take up multiple bytes
    // dpeending on its width and height
    uint8_t *character_data;

    // should equal 8.
    size_t bits_width_per_character;
    // should equal 11
    size_t bits_height_per_character;


    // Equal to the amount to subtract from the ascii character for drawing
    uint8_t subtract_from_ascii_char_index_for_drawing;

    // change magic number..
    char filename[200];
};

/**
 * Sets up the system font.
 */
int font_system_init();
struct font* font_get_system_font();

struct font* font_load(const char* filename);
struct font *font_create(uint8_t *character_data, size_t character_count, size_t bits_width_per_character, size_t bits_height_per_character, uint8_t subtract_from_ascii_char_index_for_drawing);
int font_draw(struct graphics* graphics_info, struct font* font, int screen_x, int screen_y, int character, struct framebuffer_pixel font_color);
int font_draw_text(struct graphics* graphics_info, struct font* font, int screen_x, int screen_y, const char* str, struct framebuffer_pixel font_color);
int font_draw_text_wrap(struct graphics* graphics_info, struct font* font, int screen_x, int screen_y, int width, int height, const char* str, struct framebuffer_pixel font_color);

#endif