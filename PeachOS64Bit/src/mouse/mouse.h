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

#ifndef KERNEL_MOUSE_H
#define KERNEL_MOUSE_H

#include "lib/vector/vector.h"

#define MOUSE_GRAPHIC_DEFAULT_WIDTH 10
#define MOUSE_GRAPHIC_DEFAULT_HEIGHT 10
#define MOUSE_GRAPHIC_ZINDEX 100000

enum
{
    MOUSE_NO_CLICK,
    MOUSE_LEFT_BUTTON_CLICKED,
    MOUSE_RIGHT_BUTTON_CLICKED,
    MOUSE_MIDDLE_BUTTON_CLICKED
};

typedef int MOUSE_CLICK_TYPE;

struct mouse;
typedef int (*MOUSE_INIT_FUNCTION)(struct mouse* mouse);
typedef void (*MOUSE_DRAW_FUNCTION)(struct mouse* mouse);

typedef void (*MOUSE_CLICK_EVENT_HANDLER_FUNCTION)(struct mouse* mouse, int clicked_x, int clicked_y, MOUSE_CLICK_TYPE type);
typedef void (*MOUSE_MOVE_EVENT_HANDLER_FUNCTION)(struct mouse* mouse, int moved_to_x, int moved_to_y);

struct window;
struct mouse
{
    MOUSE_INIT_FUNCTION init;
    MOUSE_DRAW_FUNCTION draw;
    char name[20];
    struct
    {
        // Current coordiantes where the mouse graphic is on the screen
        int x;
        int y;
    } coords;

    // Mouse graphics.
    struct
    {
        struct window* window;
        int width;
        int height;
    } graphic;

    struct
    {
        // Vector of MOUSE_CLICK_EVENT_HANDLER_FUNCTION
        struct vector* click_handlers;
        // Vctor of MOUSE_MOVE_EVENT_HANDLER_FUNCTION
        struct vector* move_handlers;
    } event_handlers;

    // THis is the private data for the mouse instance.
    void* private;
};

int mouse_system_load_static_drivers();
void mouse_draw(struct mouse* mouse);
void mouse_register_click_handler(struct mouse* mouse, MOUSE_CLICK_EVENT_HANDLER_FUNCTION click_handler);
void mouse_register_move_handler(struct mouse* mouse, MOUSE_MOVE_EVENT_HANDLER_FUNCTION move_handler);
void mouse_unregister_click_handler(struct mouse* mouse, MOUSE_CLICK_EVENT_HANDLER_FUNCTION click_handler);
void mouse_unregister_move_handler(struct mouse* mouse, MOUSE_MOVE_EVENT_HANDLER_FUNCTION move_handler);
void mouse_moved(struct mouse* mouse);
void mouse_click(struct mouse* mouse, MOUSE_CLICK_TYPE type);
void mouse_position_set(struct mouse* mouse, size_t x, size_t y);
int mouse_register(struct mouse* mouse);
int mouse_system_init();

#endif