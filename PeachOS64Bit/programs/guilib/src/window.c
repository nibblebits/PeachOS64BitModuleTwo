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

#include "window.h"
#include "peachos.h"

void* window_graphics(struct window* window)
{
    return peachos_window_get_graphics(window);
}

void window_redraw(struct window* window)
{
    peachos_window_redraw(window);
}


void window_redraw_region(struct window* window, int rect_x, int rect_y, int rect_width, int rect_height)
{
    peachos_window_redraw_region(rect_x, rect_y, rect_width, rect_height, window);
}

struct window* window_create(const char* title, int width, int height, int flags, int id)
{
    // Call assembler function to creaate the window on the kernel.
    struct window* win = peachos_window_create(title, width, height, flags, id);
    return win;
}



void window_title_set(struct window* window, const char* title)
{
    peachos_window_title_set(window, title);
}

int window_get_event(struct window_event* event_out)
{
    return peachos_process_get_window_event(event_out);
}
void window_set_to_receive_stdout(struct window* win)
{
    peachos_divert_stdout_to_window(win);
}