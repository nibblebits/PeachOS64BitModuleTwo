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
struct vector *windows_vector;

// close icon image
struct image *close_icon = NULL;

// Which window is currently moving
struct window *window_moving = NULL;

// Which window currently has focus
struct window *focused_window = NULL;

int window_autoincrement_id_current = 100000;

int window_system_initialize()
{
    int res = 0;

    windows_vector = vector_new(sizeof(struct window *), 10, 0);
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

struct window *window_create(struct graphics_info *graphics_info, struct font *font, const char *title, size_t x, size_t y, size_t width, size_t height, int flags, int id)
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

    struct window *window = kzalloc(sizeof(struct window));
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

    struct graphics_info *title_bar_graphics_info = NULL;
    struct graphics_info *border_left_graphics_info = NULL;
    struct graphics_info *border_right_graphics_info = NULL;
    struct graphics_info *border_bottom_graphics_info = NULL;

    if (!(flags & WINDOW_FLAG_BORDERLESS))
    {
        total_window_width_bounds += (WINDOW_BORDER_PIXEL_SIZE * 2);
        total_window_height_bounds += WINDOW_TITLE_BAR_HEIGHT + WINDOW_BORDER_PIXEL_SIZE;
        window_body_height_offset = WINDOW_TITLE_BAR_HEIGHT;
        window_body_width_offset = WINDOW_BORDER_PIXEL_SIZE;
    }

    struct graphics_info *root_graphics_info = graphics_info_create_relative(graphics_info, x, y, total_window_width_bounds, total_window_height_bounds, GRAPHICS_FLAG_DO_NOT_COPY_PIXELS);
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
    if (!(flags & WINDOW_FLAG_BORDERLESS))
    {
        title_bar_graphics_info = 
            graphics_info_create_relative(root_graphics_info, WINDOW_BORDER_PIXEL_SIZE, 0, width, WINDOW_TITLE_BAR_HEIGHT, 0);
        if (!title_bar_graphics_info)
        {
            res = -ENOMEM;
            goto out;
        }
        // click handler
        // move handler
        window->title_bar_graphics = title_bar_graphics_info;

        border_left_graphics_info = 
            graphics_info_create_relative(root_graphics_info, 0, WINDOW_TITLE_BAR_HEIGHT, WINDOW_BORDER_PIXEL_SIZE, height, 0);
        if (!border_left_graphics_info)
        {
            res = -ENOMEM;
            goto out;
        }

        struct graphics_info* border_right_graphics_info = 
            graphics_info_create_relative(root_graphics_info, total_window_width_bounds - WINDOW_BORDER_PIXEL_SIZE, WINDOW_TITLE_BAR_HEIGHT, WINDOW_BORDER_PIXEL_SIZE, height, 0);
        if (!border_right_graphics_info)
        {
            res = -ENOMEM;
            goto out;
        }

        struct graphics_info* border_bottom_graphics_info = 
            graphics_info_create_relative(root_graphics_info, 0, total_window_height_bounds-WINDOW_BORDER_PIXEL_SIZE, width, WINDOW_BORDER_PIXEL_SIZE, 0);
        if (!border_bottom_graphics_info)
        {
            res = -ENOMEM;
            goto out;
        }
    }

    struct graphics_info* window_graphics_info = graphics_info_create_relative(root_graphics_info, window_body_width_offset, window_body_height_offset, width, height, 0);
    if (!window_graphics_info)
    {
        res = -ENOMEM;
        goto out;
    }

    window->graphics = window_graphics_info;

    if (!(flags & WINDOW_FLAG_BORDERLESS))
    {
        struct framebuffer_pixel title_bar_font_color = {0};
        title_bar_font_color.red = 0xff;
        title_bar_font_color.blue = 0xff;
        title_bar_font_color.green = 0xff;

        window->title_bar_terminal = terminal_create(title_bar_graphics_info, 0, 0, total_window_width_bounds, WINDOW_TITLE_BAR_HEIGHT, font, title_bar_font_color, 0);
        if (!window->title_bar_terminal)
        {
            res = -ENOMEM;
            goto out;
        }

    }

    struct framebuffer_pixel pixel_color = {0};
    pixel_color.red = 0xff;
    window->terminal = terminal_create(window_graphics_info, 0, 0, width, height, font, pixel_color, TERMINAL_FLAG_BACKSPACE_ALLOWED);
    if (!window->terminal)
    {
        res = -ENOMEM;
        goto out;
    }

    struct framebuffer_pixel bg_color = {0};
    bg_color.red = 0xff;
    bg_color.blue = 0xff;
    bg_color.green = 0xff;
    terminal_draw_rect(window->terminal, 0, 0, width, height, bg_color);

    // Save the background of the terminal incase of backspaces
    terminal_background_save(window->terminal);

    if (flags & WINDOW_FLAG_BACKGROUND_TRANSPARENT)
    {
        terminal_transparency_key_set(window->terminal, bg_color);
    }

    if (!(flags & WINDOW_FLAG_BORDERLESS))
    {
        size_t icon_pos_x = window->title_bar_terminal->bounds.width - close_icon->width - (close_icon->width / 2);
        size_t icon_pos_y = (window->title_bar_terminal->bounds.height / 2) - (close_icon->height / 2);

        window->title_bar_components.close_btn.x = icon_pos_x;
        window->title_bar_components.close_btn.y = icon_pos_y;
        window->title_bar_components.close_btn.width = close_icon->width;
        window->title_bar_components.close_btn.height = close_icon->height;

        struct framebuffer_pixel title_bar_bg_color = {0};
        title_bar_bg_color.red = 0x00;
        title_bar_bg_color.blue = 0x00;
        title_bar_bg_color.green = 0x00;

        window_draw_title_bar(window, title_bar_bg_color);

        struct framebuffer_pixel border_color = {0};
        graphics_draw_rect(border_left_graphics_info, 0, 0, border_left_graphics_info->width, border_left_graphics_info->height, border_color);
        graphics_draw_rect(border_right_graphics_info, 0, 0, border_right_graphics_info->width, border_right_graphics_info->height, border_color);
        graphics_draw_rect(border_bottom_graphics_info, 0, 0, border_bottom_graphics_info->width, border_bottom_graphics_info->height, border_color);
    }

    // Push to the windows vector
    vector_push(windows_vector, &window);

    size_t child_count = vector_count(window->root_graphics->children);
    window_set_z_index(window, child_count+1);

    // Register the window event handler
    // TODO

    window_focus(window);

    // Redraw all graphics including our window 
    graphics_redraw_all();

out:
    if (res < 0)
    {
        if (window)
        {
            if (window->terminal)
            {
                terminal_free(window->terminal);
                window->terminal = NULL;
            }
            if (window->title_bar_terminal)
            {
                terminal_free(window->title_bar_terminal);
                window->title_bar_terminal = NULL;
            }

            vector_pop_element(windows_vector, &window, sizeof(struct window*));
            kfree(window);
            window = NULL;
        }
    }

    return window;
}