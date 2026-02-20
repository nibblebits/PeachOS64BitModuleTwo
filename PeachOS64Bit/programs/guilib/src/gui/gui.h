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

#ifndef USERSPACE_GUI_H
#define USERSPACE_GUI_H

#include "window.h"
#include "vector.h"

// Seperate header file because the function pointers
// might be used in other parts of the project
// where forward declarations may be required.
#include "event.h"
enum
{
    // Set if the GUI Must redraw its self fully.
    GUI_FLAG_MUST_DRAW = 0b00000001,
};


struct gui_event;


struct gui_element;
struct window;
struct graphics;
struct gui
{
    // GHUI FLags.
    int flags;
    // Vector of gui_element* on this gui interface
    // i.e buttons, textareas ect..
    struct vector* elements;

    // The GUI element that is in focus and should take
    // priority for events.
    struct gui_element* focused_element;

    // vector of struct event*
    struct vector* gui_events;
    
    struct 
    {
        // event handler function.
        GUI_EVENT_HANDLER_FUNCTION event;
    } handlers;
    
    // Window graphics.
    struct graphics* win_graphics;
    struct window* window;
};


/**
 * Forces the window to be bound by GUI related actions
 * such as buttons ect..
 */
struct gui* gui_bind_to_window(struct window* window, GUI_EVENT_HANDLER_FUNCTION event_handler);

int gui_process(struct gui* gui);
int gui_redraw(struct gui* gui);
void gui_element_private_set(struct gui_element* gui_element, void* private_data);
void gui_mark_for_redraw(struct gui* gui);

void gui_focus_on_element(struct gui* gui, struct gui_element* element);
struct gui_element* gui_focused_element(struct gui* gui);


#endif