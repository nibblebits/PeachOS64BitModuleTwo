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

#include "image_element.h"
#include "../image.h"
#include "../graphics.h"
#include "element.h"
#include "gui.h"
#include "memory.h"
#include "stdlib.h"
#include "stdio.h"

void gui_element_image_draw(struct gui_element *element)
{
    struct userland_gui_image_element_private *private_data = (struct userland_gui_image_element_private *)element->private;
    // Let's draw the image.
    graphics_draw_image(element->graphics, private_data->loaded_image, element->x, element->y);
}

void gui_element_image_free(struct gui_element* element)
{
    if (element->private)
        free(element->private);


    free(element);
}

struct gui_element *gui_element_image_create(struct gui *gui, struct image *img, struct gui_element *parent, int x, int y, int width, int height)
{
    int res = 0;
    struct gui_element *element = gui_element_create(gui, NULL, x, y, width, height, gui_element_image_draw, gui_element_image_free, 0);
    if (!element)
    {
        res = -1;
        goto out;
    }

    struct userland_gui_image_element_private *private_data = calloc(1, sizeof(struct userland_gui_image_element_private));
    if (!private_data)
    {
        res = -1;
        goto out;
    }

    private_data->element = element;
    private_data->loaded_image = img;

    // Ok we have the element setup the private data
    element->private = private_data;
out:
    if (res < 0)
    {
        if (element)
        {
            gui_element_free(element);
            element = NULL;
        }
    }
    return element;
}