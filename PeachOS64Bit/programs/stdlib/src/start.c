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

#include "peachos.h"
#include "window.h"
#include "stdio.h"
#include "graphics.h"
#include "font.h"
#include "image.h"
extern int main(int argc, char** argv);
struct window* win = NULL;

struct window* window_main()
{
    return win;
}

void c_start()
{
    struct process_arguments arguments;
    peachos_process_get_arguments(&arguments);

    // initialize the graphics. MOVE TO SETUP FUNC
    graphics_image_formats_init();
    // MOVE TO A CUSTOM SETUP FUNCTION, BOTH OF THESE
    font_system_init();

    // We will make the console window for this program
    // 555 will be the main ID to identify the main window here..
    // magic number for now..
    win = window_create("Window", 400, 400, 0, 555);
    if (win)
    {
        // Let's divert stdout to this window
        window_set_to_receive_stdout(win);
    }




    int res = main(arguments.argc, arguments.argv);
    if (res == 0)
    {
        
    }
}