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

#include "scrollable.h"
#include "memory.h"
#include "stdlib.h"

#include "gui/element.h"
#include "status.h"
typedef void(*GUI_ELEMENT_DRAW)(struct gui_element* elem);
typedef void(*GUI_ELEMENT_FREE)(struct gui_element* elem);

void gui_element_scrollable_draw(struct gui_element* elem)
{
    // Draw todo later..
}

void gui_element_scrollable_free(struct gui_element* elem)
{
    struct gui_scrollable_element_private* private_data = elem->private;
    if (!private_data)
        return;

    
    free(private_data);
}
struct gui_element *gui_element_scrollable_create(struct gui *gui, struct gui_element *parent, int x, int y, int width, int height, int id)
{
    int res = 0;
    struct gui_element* element = gui_element_create(gui, parent, x, y, width, height, gui_element_scrollable_draw, gui_element_scrollable_free, id);
    if (!element)
    {
        res = -ENOMEM;
        goto out;
    }

    struct gui_scrollable_element_private* private_data = calloc(1, sizeof(struct gui_scrollable_element_private));
    if (!private_data)
    {
        res = -ENOMEM;
        goto out;
    }

    private_data->scroll_y = 0;

out:
    if (res < 0)
    {
        if (element->private)
        {
            free(element->private);
            element->private = NULL;
        }
        gui_element_free(element);
        element = NULL;
    }
    return element;
}