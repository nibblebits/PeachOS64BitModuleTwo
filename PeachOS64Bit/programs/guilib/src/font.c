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

#include "font.h"
#include "graphics.h"
#include "image.h"
#include "memory.h"
#include "stdlib.h"
#include "stdio.h"
#include "vector.h"
#include "string.h"
#include "status.h"
#include <stdbool.h>

struct vector *loaded_fonts = NULL;
struct font *system_font = NULL;

int font_system_init()
{
    int res = 0;
    loaded_fonts = vector_new(sizeof(struct font *), 4, 0);
    if (!loaded_fonts)
    {
        res = -ENOMEM;
        goto out;
    }

    // Let's load the system font
    system_font = font_load("@:/sysfont.bmp");
    if (!system_font)
    {
        printf("Failed to load system font");
    }

out:
    return res;
}

struct font *font_get_system_font()
{
    return system_font;
}

/**
 * PRIVATE FUNCTION DO NOT CALL OUTSIDE FONT.C
 * use font_load
 */
struct font *font_load_from_image_file(const char *filename, size_t pixel_width, size_t pixel_height, size_t y_offset_per_character)
{
    struct image *img_font = graphics_image_load(filename);
    if (!img_font)
    {
        return NULL;
    }

    // We expect each character to be 8 pixels
    size_t characters_per_row = img_font->width / pixel_width;
    size_t total_rows = img_font->height / pixel_height;
    size_t total_characters = total_rows * characters_per_row;
    if (total_characters > 255)
    {
        // 255 LIMIT! Some of this could be empty space leading to a miscalculation of characters
        // not a problem but max will be 255
        total_characters = 255;
    }
    size_t total_required_bits_for_character_set =
        total_characters *
        pixel_width *
        pixel_height;

    // Correct conversion from bits to bytes:
    size_t total_required_bytes_for_character_set = total_required_bits_for_character_set / 8;
    if ((total_required_bits_for_character_set % 8) != 0)
    {
        total_required_bytes_for_character_set++;
    }

    size_t total_required_bits_per_character =
        pixel_width *
        pixel_height;

    size_t total_required_bytes_per_character = total_required_bits_per_character / 8;
    if ((total_required_bits_per_character % 8) != 0)
    {
        total_required_bytes_per_character++;
    }

    uint8_t *character_data = calloc(1, total_required_bytes_for_character_set);
    // we start from space.
    for (int row = 0; row < total_rows; row++)
    {
        for (int col = 0; col < characters_per_row; col++)
        {
            size_t character_index = (row * characters_per_row) + col;
            size_t starting_x = col * pixel_width;
            size_t starting_y = row * pixel_height;
            for (size_t x = 0; x < pixel_width; x++)
            {
                for (size_t y = y_offset_per_character; y < pixel_height; y++)
                {
                    size_t abs_x = starting_x + x;
                    size_t abs_y = starting_y + y;
                    image_pixel_data pixel = graphics_image_get_pixel(img_font, abs_x, abs_y);
                    if (pixel.R != 0 || pixel.B != 0 || pixel.G != 0)
                    {
                        // If it's not black, then it's a valid pixel for the character bitmap

                        // Calculate the starting position for this character's data in the array
                        size_t char_offset = character_index * total_required_bytes_per_character;
                        // Determine the bit position for this pixel within the character's bitmap
                        size_t bit_index = y * pixel_width + x;
                        // Calculate which byte and bit within that byte to set
                        size_t byte_index = char_offset + (bit_index / 8);
                        uint8_t bit_mask = 1 << (bit_index % 8);
                        character_data[byte_index] |= bit_mask;
                    }
                }
            }
        }
    }

    return font_create(character_data, total_characters,
                       pixel_width,
                       pixel_height, FONT_IMAGE_DRAW_SUBTRACT_FROM_INDEX);
}

struct font *font_get_loaded_font(const char *filename)
{
    struct font *font = NULL;
    size_t total_fonts = vector_count(loaded_fonts);
    for (size_t i = 0; i < total_fonts; i++)
    {
        vector_at(loaded_fonts, i, &font, sizeof(font));
        if (font)
        {
            if (strncmp(font->filename, filename, sizeof(font->filename)) == 0)
            {
                // We found the font
                return font;
            }
        }
    }

    return NULL;
}

struct font *font_load(const char *filename)
{
    struct font *loaded_font = font_get_loaded_font(filename);
    if (loaded_font)
    {
        return loaded_font;
    }

    // We only support image fonts at the moment
    loaded_font = font_load_from_image_file(filename, FONT_IMAGE_CHARACTER_WIDTH_PIXEL_SIZE, FONT_IMAGE_CHARACTER_HEIGHT_PIXEL_SIZE, FONT_IMAGE_CHARACTER_Y_OFFSET);
    if (loaded_font)
    {
        // Copy the identifying filename.
        strncpy(loaded_font->filename, filename, sizeof(loaded_font->filename));
        // push it to the vector so it doesnt have to be loaded again
        vector_push(loaded_fonts, &loaded_font);
    }

    return loaded_font;
}

struct font *font_create(uint8_t *character_data, size_t character_count, size_t bits_width_per_character, size_t bits_height_per_character, uint8_t subtract_from_ascii_char_index_for_drawing)
{
    struct font *font = calloc(1, sizeof(struct font));
    font->character_count = character_count;
    font->character_data = character_data;
    font->bits_width_per_character = bits_width_per_character;
    font->bits_height_per_character = bits_height_per_character;
    font->subtract_from_ascii_char_index_for_drawing = subtract_from_ascii_char_index_for_drawing;
    return font;
}

int font_draw_from_index(struct graphics *graphics_info, struct font *font, int screen_x, int screen_y, int index_character, struct framebuffer_pixel font_color)
{
    int res = 0;
    if (!font)
    {
        res = -EINVARG;
        goto out;
    }

    if (index_character > font->character_count)
    {
        res = 0;
        goto out;
    }

    // Readjust the character so its index is correct within the data array

    size_t total_required_bits_per_character =
        font->bits_width_per_character *
        font->bits_height_per_character;

    size_t total_required_bytes_per_character = total_required_bits_per_character / 8;
    if ((total_required_bits_per_character % 8) != 0)
    {
        total_required_bytes_per_character++;
    }

    size_t character_index = index_character * total_required_bytes_per_character;
    for (size_t x = 0; x < font->bits_width_per_character; x++)
    {
        for (size_t y = 0; y < font->bits_height_per_character; y++)
        {
            size_t char_offset = character_index;
            // Determine the bit position for this pixel within the character's bitmap
            size_t bit_index = y * font->bits_width_per_character + x;
            // Calculate which byte and bit within that byte to set
            size_t byte_index = char_offset + (bit_index / 8);
            if ((font->character_data[byte_index] >> (bit_index % 8)) & 0x01)
            {
                size_t abs_x = screen_x + x;
                size_t abs_y = screen_y + y;

                // bit is set
                graphics_draw_pixel(graphics_info, abs_x, abs_y, font_color);
            }
        }
    }

    // TODO FOR IMPROVEMENT, REDRAW THE REGION 
out:
    return res;
}

int font_draw(struct graphics *graphics_info, struct font *font, int screen_x, int screen_y, int character, struct framebuffer_pixel font_color)
{
    character -= (int)font->subtract_from_ascii_char_index_for_drawing;
    return font_draw_from_index(graphics_info, font, screen_x, screen_y, character, font_color);
}

int font_draw_text_wrap(struct graphics* graphics_info, struct font* font, int screen_x, int screen_y, int width, int height, const char* str, struct framebuffer_pixel font_color)
{
    int res =0;

    if (!font)
    {
        font = font_get_system_font();
    }

    int current_x = screen_x;
    int current_y = screen_y;
    int ending_x = screen_x + width;
    int ending_y = screen_y + height;

    // We want to draw each character but wrap around when we reach width
    const char* c = str;
    while(*c)
    {
        // If carraige return then we need to go to next line
        if (*c == 0x0D)
        {   
            current_x = screen_x;
            current_y += font->bits_height_per_character;
            // Continue so we dont end up printing the 
            // carriage return.
            continue;
        }
        
        if (current_x >= ending_x)
        {
            // Loop back 
            current_x = screen_x;
            // Next row
            current_y += font->bits_height_per_character; 
        }
        if (current_y >= ending_y)
        {
            // overflow refuse to draw
            break;
        }
        
        font_draw(graphics_info, font, current_x, current_y, *c, font_color);
        // next character
        c++;
        // inhance the pixels
        current_x += font->bits_width_per_character;
    }

    return res;
}
int font_draw_text(struct graphics *graphics_info, struct font *font, int screen_x, int screen_y, const char *str, struct framebuffer_pixel font_color)
{
    int res = 0;
    int x = screen_x;
    int y = screen_y;


    if (!font)
    {
        // null font default to system
        font = font_get_system_font();
    }

    if (!font)
    {
        return -1;
    }
    
    while (*str != 0)
    {
        res = font_draw(graphics_info, font, x, y, *str, font_color);
        if (res < 0)
        {
            break;
        }

        x += font->bits_width_per_character;
        str++;
    }
    return res;
}