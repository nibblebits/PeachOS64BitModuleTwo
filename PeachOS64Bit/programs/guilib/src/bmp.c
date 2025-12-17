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

#include "bmp.h"
#include "image.h"
#include "stdlib.h"
#include "string.h"
#include "memory.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "status.h"

typedef struct image *(*image_load_function)(void *memory, size_t size);
typedef void (*image_free_function)(struct image *image);
typedef int (*image_format_register_function)(struct image_format *format);
typedef void (*image_format_unregister_function)(struct image_format *format);

// Loads an uncompressed 24-bit BMP
struct image *bmp_img_load(void *memory, size_t size)
{
    int res = 0;
    struct image *img = NULL;
    struct bmp_header       *header           = NULL;
    struct bmp_image_header *bmp_image_header = NULL;

    // Basic size check for the BMP file header
    if (size < sizeof(struct bmp_header))
    {
        res = -EINFORMAT;
        goto out;
    }

    header = (struct bmp_header *)memory;

    // Check BMP magic signature "BM"
    if (memcmp(header->bf_type, BMP_SIGNATURE, sizeof(header->bf_type)) != 0)
    {
        // Not a BMP
        res = -EINFORMAT;
        goto out;
    }

    // The DIB header (bmp_image_header) starts right after bmp_header
    if (header->bf_offbits >= size)
    {
        // The stated pixel offset is outside the file, invalid
        res = -EINFORMAT;
        goto out;
    }

    bmp_image_header = (struct bmp_image_header *)((uintptr_t)header + sizeof(struct bmp_header));

    // Allocate our image structure
    img = calloc(1, sizeof(struct image));
    if (!img)
    {
        res = -ENOMEM;
        goto out;
    }

    // Check compression
    if (bmp_image_header->bi_compression != BMP_COMPRESSION_UNCOMPRESSED)
    {
        // We only support uncompressed BMP
        res = -EUNIMP;
        goto out;
    }

    // Bits per pixel
    uint16_t bits_per_pixel  = bmp_image_header->bi_bits_per_pixel;
    if (bits_per_pixel != 24)
    {
        // Only 24-bit BMP is supported
        res = -EUNIMP;
        goto out;
    }
    uint16_t bytes_per_pixel = bits_per_pixel / 8; // 3 for 24-bit

    // BMP can store height as negative, indicating top-down.
    // If bi_height > 0, it's a bottom-up BMP (rows in reverse).
    // If bi_height < 0, it's a top-down BMP (we can read rows in normal order).
    bool bottom_up = (bmp_image_header->bi_height > 0);
    int32_t height = bottom_up
                         ? bmp_image_header->bi_height
                         : -(bmp_image_header->bi_height);

    int32_t width = bmp_image_header->bi_width;
    if (width <= 0 || height <= 0)
    {
        // Invalid dimensions
        res = -EINFORMAT;
        goto out;
    }

    // Prepare the image struct
    img->width  = width;
    img->height = height;

    // Our kernel image API presumably wants 32-bit RGBA or at least 24-bit storage
    // We'll store it in the given struct image_pixel_data, presumably {R,G,B,A?}
    size_t pixel_data_size = (size_t)width * (size_t)height * sizeof(image_pixel_data);
    img->data = calloc(1, pixel_data_size);
    if (!img->data)
    {
        res = -ENOMEM;
        goto out;
    }

    // Figure out row sizes (BMP rows are 4-byte aligned)
    size_t raw_row_size    = (size_t)width * bytes_per_pixel;    // e.g. width*3 for 24-bit
    size_t padded_row_size = (raw_row_size + 3) & ~3;            // round up to multiple of 4

    // Make sure we won't read outside the file
    // row count is 'height'; each row is padded_row_size bytes
    if ((header->bf_offbits + (padded_row_size * (size_t)height)) > size)
    {
        res = -EINFORMAT;
        goto out;
    }

    // Pointer to the first pixel data
    uint8_t *bmp_first_pixel_ptr = (uint8_t *)memory + header->bf_offbits;

    // Read rows
    // If bottom_up = true, the first row in the file is actually the BOTTOM of the image
    for (int row = 0; row < height; row++)
    {
        // row_ptr in the BMP data
        // the file’s “first” row is row=0 from the offset
        uint8_t *row_ptr = bmp_first_pixel_ptr + (row * padded_row_size);

        // But we need to place it in the correct row in our output
        int dest_row = bottom_up ? (height - 1 - row) : row; // flip if bottom-up

        for (int col = 0; col < width; col++)
        {
            // Each pixel is 3 bytes: B, G, R for 24-bit BMP
            uint8_t *bmp_pixel = row_ptr + (col * 3);
            uint8_t B = bmp_pixel[0];
            uint8_t G = bmp_pixel[1];
            uint8_t R = bmp_pixel[2];

            // Write into our image buffer
            size_t pixel_index = (dest_row * (size_t)width) + (size_t)col;
            image_pixel_data *out_pix = &((image_pixel_data *)img->data)[pixel_index];

            out_pix->R = R;
            out_pix->G = G;
            out_pix->B = B;
            // Zero alpha..
             out_pix->A = 0; 
        }
    }

out:
    if (res < 0)
    {
        // On error, free if allocated
        if (img)
        {
            if (img->data)
                free(img->data);
            free(img);
            img = NULL;
        }
    }
    return img;
}

// Simple free function
void bmp_img_free(struct image *image)
{
    if (!image)
        return;

    if (image->data)
        free(image->data);

    free(image);
}

// Register/unregister stubs
int bmp_img_format_register(struct image_format *format)
{
    return 0;
}

void bmp_img_format_unregister(struct image_format *format)
{
}

// The exported format object
struct image_format bmp_img_format = {
    .mime_type               = {"image/bmp"},
    .image_load_function     = bmp_img_load,
    .image_free_function     = bmp_img_free,
    .on_register_function    = bmp_img_format_register,
    .on_unregister_function  = bmp_img_format_unregister
};

struct image_format *graphics_image_format_bmp_setup()
{
    return &bmp_img_format;
}
