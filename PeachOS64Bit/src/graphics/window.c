#include "graphics/window.h"
#include "graphics/graphics.h"
#include "lib/vector/vector.h"
#include "memory/heap/kheap.h"
#include "keyboard/keyboard.h"
// include the mouse mouse.h
#include "memory/memory.h"
#include "string/string.h"
#include "graphics/font.h"
#include "task/process.h"
// include tsc.h
#include "status.h"
#include "kernel.h"

// vector of struct window*
struct vector* windows_vector;

// close icon image
struct image* close_icon = NULL;

// Which window is currently moving
struct window* window_moving = NULL;

// Which window currently has focus
struct window* focused_window = NULL;

int window_autoincrement_id_current = 100000;

int window_system_initialize()
{
    int res = 0;

    windows_vector = vector_new(sizeof(struct window*), 10, 0);
    if (!windows_vector)
    {
        res = -ENOMEM;
        goto out;
    }

    close_icon = graphics_image_load("@:/clsicon.bmp");
    if (!close_icon)
    {
        res = -EIO;
        goto out;
    }

    window_moving = NULL;
    focused_window = NULL;
    window_autoincrement_id_current = 100000;
out:
    return res;
}

int window_system_initialize_stage2()
{
    // Register the mouse move and click handlers TODO
    // Register keyboard listener...
    return 0;
}

struct window* window_create(struct graphics_info* graphics_info, struct font* font, const char* title, size_t x, size_t y, size_t width, size_t height, int flags, int id)
{
    int res = 0;
    if (!windows_vector)
    {
        panic("Window system was not initialized\n");
    }

    if (width < 1 || height < 1)
    {
        res = -EINVARG;
        goto out;
    }


    if (!font)
    {
        // No font default to the system font
        font = font_get_system_font();
    }

    struct window* window = kzalloc(sizeof(struct window));
    if (!window)
    {
        res = -ENOMEM;
        goto out;
    }

    if (id == -1)
    {
        id = window_autoincrement_id_current;
        window_autoincrement_id_current++;
    }

    strncpy(window->title, title, sizeof(window->title));
    window->x = x;
    window->y = y;
    window->width = width;
    window->height = height;
    window->flags = flags;
    window->id = id;

    // setup event handler function pointer vector
    window->event_handlers.handlers = vector_new(sizeof(WINDOW_EVENT_HANDLER), 4, 0);

    size_t total_window_width_bounds = width;
    size_t total_window_height_bounds = height;
    size_t window_body_height_offset = 0;
    size_t window_body_width_offset = 0;

    struct graphics_info* title_bar_graphics_info = NULL;
    struct graphics_info* border_left_graphics_info = NULL;
    struct graphics_info* border_right_graphics_info = NULL;
    struct graphics_info* border_bottom_graphics_info = NULL;

    if (!(flags & WINDOW_FLAG_BORDERLESS))
    {
        total_window_width_bounds += (WINDOW_BORDER_PIXEL_SIZE * 2);
        total_window_height_bounds += WINDOW_TITLE_BAR_HEIGHT + WINDOW_BORDER_PIXEL_SIZE;
        window_body_height_offset = WINDOW_TITLE_BAR_HEIGHT;
        window_body_width_offset = WINDOW_BORDER_PIXEL_SIZE;
    }

    struct graphics_info* root_graphics_info = graphics_info_create_relative(graphics_info, x, y, total_window_width_bounds, total_window_height_bounds, GRAPHICS_FLAG_DO_NOT_COPY_PIXELS);
    if (!root_graphics_info)
    {
        res = -ENOMEM;
        goto out;
    }

    if (flags & WINDOW_FLAG_BACKGROUND_TRANSPARENT)
    {
        struct framebuffer_pixel transparency_key = {0};
        transparency_key.blue = 0xff;
        transparency_key.green = 0xff;
        transparency_key.red = 0xff;
        graphics_transparency_key_set(root_graphics_info, transparency_key);
        graphics_draw_rect(root_graphics_info, 0, 0, root_graphics_info->width, root_graphics_info->height, transparency_key);
    }

    window->root_graphics = root_graphics_info;
out:

}