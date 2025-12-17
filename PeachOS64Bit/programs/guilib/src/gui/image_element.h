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

#ifndef USERLAND_GUI_IMAGE_ELEMENT_H
#define USERLAND_GUI_IMAGE_ELEMENT_H


struct gui;
struct gui_element;
struct image;
struct userland_gui_image_element_private
{
    // The true loaded image element
    struct image *loaded_image;

    // Pointer to our element
    struct gui_element *element;
};

struct gui_element *gui_element_image_create(struct gui *gui, struct image *img, struct gui_element *parent, int x, int y, int width, int height);

#endif