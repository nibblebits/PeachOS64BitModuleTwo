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

#ifndef USERLAND_GUI_ELEMENT_BUTTON
#define USERLAND_GUI_ELEMENT_BUTTON
struct gui;
struct gui_element;
struct font;

/**
 *  We will be a child of the plane element.
 */
struct gui_button_element_private
{
    char button_text[120];

    // Our gui_element
    struct gui_element* element;


    // The font used for the text in this button
    struct font* font;
};

struct gui_element *gui_element_button_create(struct gui *gui, struct gui_element *parent, int x, int y, int width, int height, const char *text, int id);


#endif