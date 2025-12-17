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

#include "image.h"

#include "graphics.h"
#include "stdlib.h"

// file
#include "file.h"

// vector
#include "vector.h"

// status
#include "status.h"

// string
#include "string.h"

#include "memory.h"

#include "bmp.h"

struct vector *image_formats;

struct image_format *graphics_image_format_get(const char *mime_type)
{
    struct image_format *format = NULL;
    for (size_t i = 0; i < vector_count(image_formats); i++)
    {
        struct image_format *current_format = NULL;
        int res = vector_at(image_formats, i, &current_format, sizeof(struct image_format *));
        if (res < 0)
        {
            break;
        }

        if (current_format && strncmp(current_format->mime_type, mime_type, sizeof(current_format->mime_type)) == 0)
        {
            format = current_format;
            break;
        }
    }

    return format;
}

/**
 * Finds a loaded image format that can translate the given image memory into pixels
 * and attempts to load the image from memory, returns NULL On failure
 *
 */
struct image *graphics_image_load_from_memory(void *memory, size_t max)
{
    struct image *image_out = NULL;
    size_t total_formats = vector_count(image_formats);
    for (size_t i = 0; i < total_formats; i++)
    {
        struct image_format *_format = NULL;
        int res = vector_at(image_formats, i, &_format, sizeof(_format));
        if (res < 0)
        {
            break;
        }

        image_out = _format->image_load_function(memory, max);
        if (image_out)
        {
            image_out->format = _format;
            break;
        }
    }

    return image_out;
}

image_pixel_data graphics_image_get_pixel(struct image *image, int x, int y)
{
    image_pixel_data pixel_data = image->data[(y * image->width) + x];
    return pixel_data;
}

struct image *graphics_image_load(const char *path)
{
    struct image *img = NULL;
    void *img_memory = NULL;
    int fd = 0;
    int res = 0;

    fd = fopen(path, "r");
    if (fd < 0)
    {
        res = fd;
        goto out;
    }

    // get file size
    struct file_stat stat;
    res = fstat(fd, &stat);
    if (res < 0)
    {
        goto out;
    }

    img_memory = calloc(1, stat.filesize);
    if (!img_memory)
    {
        goto out;
    }

    res = fread(img_memory, stat.filesize, 1, fd);
    if (res < 0)
    {
        goto out;
    }

    // Let's try to load the image from this memory
    img = graphics_image_load_from_memory(img_memory, stat.filesize);
    if (!img)
    {
        res = -EINFORMAT;
        goto out;
    }

out:
    // We can close the file and free the memory now
    fclose(fd);
    if (img_memory)
    {
        free(img_memory);
    }
    img_memory = NULL;
    if (res < 0)
    {
        if (img)
        {
            graphics_image_free(img);
        }

        // Error means we should not return an image so must be null.
        // if it was not null before it has been freed above..
        img = NULL;
    }
    return img;
}

bool image_in_bounds(struct image* img, size_t x, size_t y)
{
    // TODO change the silly 32 bit types.
    return x < img->width && y < img->height;
}



void graphics_draw_image(struct graphics* graphics, struct image* img, int rel_x, int rel_y)
{
    // lets loop through all the pixels and draw them
    struct framebuffer_pixel* pixels = graphics_get_pixel_buffer(graphics);
    if(!pixels)
        return;
    
    for(size_t x = 0; x < img->width; x++)
    {
        for(size_t y = 0; y < img->height; y++)
        {
            size_t ox = rel_x + x;
            size_t oy = rel_y + y;
            // Did we overflow any out buffer?
            if (!graphics_in_bounds(graphics, ox, oy))
            {
                continue;
            }
            
            struct framebuffer_pixel* pixel_out = &graphics->pixels[oy * graphics->width +ox];
            image_pixel_data* pixel_in =  &img->data[y * img->width + x];
            // copy the pixel over
            pixel_out->red = pixel_in->R;
            pixel_out->green = pixel_in->G;
            pixel_out->blue = pixel_in->B;
            // Pixel now written to the graphic.
        }
    }
}

void graphics_image_free(struct image *image)
{
    // Call the image free function incase the format made some private data related to this memory
    // that needs to be freed.
    image->format->image_free_function(image);

    // The format that created the image owns the memory of "image" so should've freed it.
}

int graphics_image_format_register(struct image_format *format)
{
    int res = 0;
    struct image_format *existing_format = graphics_image_format_get(format->mime_type);
    if (existing_format)
    {
        res = -EISTKN;
        goto out;
    }

    res = vector_push(image_formats, &format);
    if (res < 0)
    {
        goto out;
    }

    res = format->on_register_function(format);
    if (res < 0)
    {
        goto out;
    }
out:

    if (res < 0)
    {
        if (format)
        {
            graphics_image_format_unload(format);
        }
    }
    return res;
}

int graphics_image_formats_init()
{
    int res = 0;
    image_formats = vector_new(sizeof(struct image_format *), 10, VECTOR_NO_FLAGS);
    if (!image_formats)
    {
        res = -ENOMEM;
        goto out;
    }
    res = graphics_image_formats_load();
    if (res < 0)
    {
        goto out;
    }

out:
    if (res < 0)
    {
        if (image_formats)
        {
            vector_free(image_formats);
        }
    }
    return res;
}
int graphics_image_formats_load()
{
    graphics_image_format_register(graphics_image_format_bmp_setup());
    return 0;
}

void graphics_image_format_unload(struct image_format *format)
{
    if (format->on_unregister_function)
    {
        format->on_unregister_function(format);
    }
}

void graphics_image_formats_unload()
{
    // Loop through all elements and unload the image formats
    // pop each one from vector when completed
    while (vector_count(image_formats) > 0)
    {
        struct image_format *format = NULL;
        vector_back(image_formats, &format, sizeof(struct image_format *));
        if (format)
        {
            graphics_image_format_unload(format);
        }

        vector_pop(image_formats);
    }
}
