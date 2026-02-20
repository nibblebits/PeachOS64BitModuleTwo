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

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "lib/vector/vector.h"
#define KEYBOARD_CAPS_LOCK_ON 1
#define KEYBOARD_CAPS_LOCK_OFF 0

typedef int KEYBOARD_CAPS_LOCK_STATE;

typedef int KEYBOARD_EVENT_TYPE;
enum
{
    KEYBOARD_EVENT_INVALID,
    KEYBOARD_EVENT_KEY_PRESS,
    KEYBOARD_EVENT_CAPS_LOCK_CHANGE,
};

struct keyboard_event
{
    KEYBOARD_EVENT_TYPE type;
    union 
    {
        struct
        {
            int key;
        } key_press;

        struct
        {
            KEYBOARD_CAPS_LOCK_STATE state;
        } caps_lock;
    } data;
    struct keyboard* keyboard;
};

typedef void (*KEYBOARD_EVENT_LISTENER_ON_EVENT)(struct keyboard* keyboard, struct keyboard_event* event);

struct keyboard_listener
{
    KEYBOARD_EVENT_LISTENER_ON_EVENT on_event;
};

struct process;

typedef int (*KEYBOARD_INIT_FUNCTION)();
struct keyboard
{
    KEYBOARD_INIT_FUNCTION init;
    char name[20];    

    KEYBOARD_CAPS_LOCK_STATE capslock_state;

    // vector of struct keyboard_listener*
    struct vector* key_listeners;
    struct keyboard* next;
};

void keyboard_init();
void keyboard_backspace(struct process* process);
void keyboard_push(char c);
char keyboard_pop();
int keyboard_insert(struct keyboard* keyboard);
void keyboard_set_capslock(struct keyboard* keyboard, KEYBOARD_CAPS_LOCK_STATE state);
KEYBOARD_CAPS_LOCK_STATE keyboard_get_capslock(struct keyboard* keyboard);
int keyboard_register_handler(struct keyboard* keyboard, struct keyboard_listener keyboard_listener);
struct keyboard* keyboard_default();

struct keyboard_listener *keyboard_get_listener_ptr(struct keyboard *keyboard, struct keyboard_listener keyboard_listener);
int keyboard_unregister_handler(struct keyboard* keyboard, struct keyboard_listener keyboard_listener);
void keyboard_push_event_to_listeners(struct keyboard* keyboard, struct keyboard_event* event);
#endif