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

#ifndef USERLAND_GRAPHICS_IMAGE_H
#define USERLAND_GRAPHICS_IMAGE_H
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
struct image_format;
struct graphics;
typedef union image_pixel_data {
    uint32_t data;           // Full 32-bit data (e.g., RGBA packed)
    struct {
        uint8_t R;           // Red channel
        uint8_t G;           // Green channel
        uint8_t B;           // Blue channel
        uint8_t A;           // Alpha channel
    };
} image_pixel_data;

struct image
{
    uint32_t width;
    uint32_t height;

    // RGB data ( can be casted to a framebuffer_pixel )
    image_pixel_data* data;


    // Private data related to the image format
    // known to the handler that loaded the image
    void* private;

    // A pointer to the format that loaded this image.
    struct image_format* format;
}; 

typedef struct image* (*image_load_function)(void* memory, size_t size);
typedef void (*image_free_function)(struct image* image);
typedef int (*image_format_register_function)(struct image_format* format);
typedef void (*image_format_unregister_function)(struct image_format* format);

#define IMAGE_FORMAT_MAX_MIME_TYPE 64
struct image_format
{   
    char mime_type[IMAGE_FORMAT_MAX_MIME_TYPE];
    image_load_function image_load_function;
    image_free_function image_free_function;

    image_format_register_function on_register_function;
    image_format_unregister_function on_unregister_function;

    // Private data the image format owns
    void* private;
};

int graphics_image_formats_init();
int graphics_image_formats_load();
void graphics_image_formats_unload();

int graphics_image_format_register(struct image_format* format);
void graphics_image_format_unload(struct image_format* format);
struct image_format* graphics_image_format_get(const char* mime_type);
struct image *graphics_image_load_from_memory(void *memory, size_t max);
struct image* graphics_image_load(const char* path);
image_pixel_data graphics_image_get_pixel(struct image *image, int x, int y);
void graphics_image_free(struct image* image);
bool image_in_bounds(struct image* img, size_t x, size_t y);
void graphics_draw_image(struct graphics* graphics, struct image* img, int rel_x, int rel_y);;

#endif