/*
 * PeachOS 64-Bit Kernel Project
 * Copyright (C) 2026 Daniel McCarthy <daniel@dragonzap.com>
 *
 * This file is part of the PeachOS 64-Bit Kernel.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <https://www.gnu.org/licenses/>.
 *
 * For full source code, documentation, and structured learning,
 * see the official kernel development course part one:
 * https://dragonzap.com/course/developing-a-multithreaded-kernel-from-scratch
 *
 * Get part one and part two module one, module two all peachos courses (69 hours of content): https://dragonzap.com/offer/kernel-development-from-scratch-69-hours
 *
 * Get the part two course module one and two: https://dragonzap.com/offer/developing-a-multithreaded-kernel-from-scratch-part-two-full-series
 */

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

#include "graphics.h"
#include "peachos.h"
#include "image.h"
void* graphics_create_relative(void* parent_graphics, size_t x, size_t y, size_t width, size_t height)
{
    return peachos_graphics_create(x, y, width, height, parent_graphics);
}

struct framebuffer_pixel* graphics_get_pixel_buffer(struct graphics* graphics)
{
    // Don't waste CPU calling kernel if we have the buffer
    if (graphics->pixels)
    {
        return graphics->pixels;
    }
    graphics->pixels = (struct framebuffer_pixel*) peachos_graphic_pixels_get(graphics);
    return graphics->pixels;
}

bool graphics_in_bounds(struct graphics* graphics, size_t x, size_t y)
{
    return x < graphics->width && y < graphics->height;
}

void graphics_draw_pixel(struct graphics *graphics_info, uint32_t x, uint32_t y, struct framebuffer_pixel pixel)
{
   
    // NO IGNORE OR TRANSPARENCY COLORS AVAILABLE IN USERLAND YET

    if (x < graphics_info->width && y < graphics_info->height)
    {
        graphics_info->pixels[y * graphics_info->width + x] = pixel;
    }
}

void graphics_draw_rect(struct graphics* graphics_info, uint32_t x, uint32_t y, uint32_t width, uint32_t height, struct framebuffer_pixel pixel_color)
{
    for (uint32_t lx = 0; lx  < width; lx++)
    {
        for (uint32_t ly = 0; ly < height; ly++)
        {
            uint32_t ax = x + lx;
            uint32_t ay = y + ly;
            graphics_draw_pixel(graphics_info, ax, ay, pixel_color);
        }
    }
}