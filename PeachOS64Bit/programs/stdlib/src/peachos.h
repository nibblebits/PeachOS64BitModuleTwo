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

#ifndef PEACHOS_H
#define PEACHOS_H
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

struct file_stat;

struct command_argument
{
    char argument[512];
    struct command_argument* next;
};

struct process_arguments
{
    int argc;
    char** argv;
};

struct window;
struct window_event;

void print(const char* filename);
int peachos_getkey();

void* peachos_malloc(size_t size);
void* peachos_realloc(void* old_ptr, size_t new_size);

void peachos_free(void* ptr);
void peachos_putchar(char c);
int peachos_getkeyblock();
void peachos_terminal_readline(char* out, int max, bool output_while_typing);
void peachos_process_load_start(const char* filename);
struct command_argument* peachos_parse_command(const char* command, int max);
void peachos_process_get_arguments(struct process_arguments* arguments);
int peachos_system(struct command_argument* arguments);
int peachos_system_run(const char* command);
void peachos_exit();


/**
 * Long is used to simplify the communication to the kernel
 * returns a pointer to the user space created window.
 * Accessible by the user land
 */
void* peachos_window_create(const char* title, long width, long height, long flags, long id);
void peachos_window_redraw_region(long rel_x, long rel_y, long rel_width, long rel_height, struct window* window);

void peachos_divert_stdout_to_window(struct window* window);


int peachos_process_get_window_event(struct window_event* window_event);

/**
 * Gets the graphics of the window
 */
void* peachos_window_get_graphics(struct window* window);

/**
 * Creates new graphics relative to the parent
 * \param x relative x to parent x
 * \param y relative y to parent y
 * 
 * ect....
 */
void* peachos_graphics_create(size_t x, size_t y, size_t width, size_t height, void* parent_graphics);


/**
 * Get the pixels buffer of a graphical entity
 */
void* peachos_graphic_pixels_get(void* graphics);


/**
 * Redraws the window 
 */
void peachos_window_redraw(struct window* window);


// long as its easier. stdlib will follow correct specification
long peachos_fopen(const char* filename, const char* mode);
void peachos_fclose(long fd);
long peachos_fread(void* buffer, size_t size, size_t count, long fd);
long peachos_fseek(long fd, long offset, long whence);
long peachos_fstat(long fd, struct file_stat* file_stat_out);
/*
* 
 * Found in start.c
 */
struct window* window_main();

void peachos_udelay(uint64_t microseconds);

// Updats the title of a window.
void peachos_window_title_set(struct window* window, const char* title);

#endif