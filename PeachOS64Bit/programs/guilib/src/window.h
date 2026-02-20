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

#ifndef STDLIB_WINDOW_H
#define STDLIB_WINDOW_H

/**
 * Not strictly part of the C STDLIB
 */

#define WINDOW_MAX_TITLE 128
enum
{
    WINDOW_EVENT_TYPE_NULL,
    WINDOW_EVENT_TYPE_FOCUS,
    WINDOW_EVENT_TYPE_LOST_FOCUS,
    WINDOW_EVENT_TYPE_MOUSE_MOVE,
    WINDOW_EVENT_TYPE_MOUSE_CLICK,
    // Close the window
    WINDOW_EVENT_TYPE_WINDOW_CLOSE,
    WINDOW_EVENT_TYPE_KEY_PRESS
};

// note: this is a user land event structure
// the kernel structure differs.
// seek window_event_userland in the kernel
// structures must match.
struct window_event
{
    int type;

    // Id of the window in question.
    int win_id;

    // Warning: set to NULL if you wont be sure if the window will exist
    // at the time you want to use the event. this will ensure
    // any user of the event will understand
    struct window *window;

    union
    {
        struct
        {
            // No extras for focus
        } focus;

        struct
        {
            int x;
            int y;
        } move;

        struct
        {
            int x;
            int y;
        } click;
        
        struct
        {
            int key;
        } keypress;

    } data;
};

// User space windows!
// Don't confuse with kernel space, the window
// structure is much more advanced.
struct window
{
    char title[WINDOW_MAX_TITLE];
    int width;
    int height;
};

/**
 * Gets the graphics of a window
 */
void *window_graphics(struct window *window);

/**
 * Returns the window that was created or NULL on failure.
 * NOTE: This is a user space equivilant of the window thats accessible
 * not the true kernel structures that represent the window internals.
 */
struct window *window_create(const char *title, int width, int height, int flags, int id);

/**
 * Gets the next window event returns zero if success negative on failure.
 *
 */
int window_get_event(struct window_event *event_out);

/**
 * Updates the title of the window
 */
void window_title_set(struct window* window, const char* title);

/**
 * When called the new window will receive all prints going forward.
 */
void window_set_to_receive_stdout(struct window *win);

void window_redraw(struct window *window);
void window_redraw_region(struct window *window, int rect_x, int rect_y, int rect_width, int rect_height);

void *window_graphics(struct window *window);
struct window *window_focused();

#endif