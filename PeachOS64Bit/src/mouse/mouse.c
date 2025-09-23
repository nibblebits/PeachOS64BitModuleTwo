#include "mouse/mouse.h"
#include "lib/vector/vector.h"
#include "graphics/graphics.h"
#include "graphics/window.h"
#include "kernel.h"
#include "status.h"

// Holds all the loaded mouse drivers
struct vector* mouse_driver_vector = NULL;

int mouse_system_load_static_drivers()
{
    int res = 0;
    // TODO: Register the ps2_mouse;
    return res;
}

int mouse_system_init()
{
    int res = 0;
    mouse_driver_vector = vector_new(sizeof(struct mouse*), 4, 0);
    if (!mouse_driver_vector)
    {
        res = -ENOMEM;
        goto out;
    }

out:   
    return res;
}

void mouse_draw_default_impl(struct mouse* mouse)
{
    struct framebuffer_pixel pixel_color = {0};
    pixel_color.red = 0xf3;
    
    struct terminal* win_term = window_terminal(mouse->graphic.window);
    terminal_draw_rect(win_term, 0, 0, win_term->bounds.width, win_term->bounds.height, pixel_color);  
}

int mouse_register(struct mouse* mouse)
{
    int res = 0;
    if (!mouse_driver_vector)
    {
        panic("Mouse system was not initialized yet\n");
    }

    if (!mouse)
    {
        res = -EINVARG;
        goto out;
    }

    mouse->event_handlers.click_handlers = vector_new(sizeof(MOUSE_CLICK_EVENT_HANDLER_FUNCTION), 4, 0);
    if (!mouse->event_handlers.click_handlers)
    {
        res = -ENOMEM;
        goto out;
    }

    mouse->event_handlers.move_handlers = vector_new(sizeof(MOUSE_MOVE_EVENT_HANDLER_FUNCTION), 4, 0);
    if (!mouse->event_handlers.move_handlers)
    {
        res = -ENOMEM;
        goto out;
    }

    res = mouse->init(mouse);
    if (res < 0)
    {
        goto out;
    }

    struct graphics_info* screen_graphics = graphics_screen_info();
    mouse->coords.x = screen_graphics->width / 2;
    mouse->coords.y = screen_graphics->height / 2;
    if (!mouse->draw)
    {
        mouse->draw = mouse_draw_default_impl;
        mouse->graphic.width = MOUSE_GRAPHIC_DEFAULT_WIDTH;
        mouse->graphic.height = MOUSE_GRAPHIC_DEFAULT_HEIGHT;
    }

    if (mouse->graphic.width <= 0 ||
        mouse->graphic.height <= 0)
    {
        res = -EINVARG;
        goto out;
    }

    if (!mouse->graphic.window)
    {
        mouse->graphic.window = window_create(screen_graphics, NULL, "", mouse->coords.x, mouse->coords.y, mouse->graphic.width, mouse->graphic.height, WINDOW_FLAG_BACKGROUND_TRANSPARENT | WINDOW_FLAG_BORDERLESS | WINDOW_FLAG_CLICK_THROUGH, -1);
        window_set_z_index(mouse->graphic.window, MOUSE_GRAPHIC_ZINDEX);
    }

    mouse->draw(mouse);
    vector_push(mouse_driver_vector, &mouse);
out:
    return res;
}

void mouse_position_set(struct mouse* mouse, size_t x, size_t y)
{
    mouse->coords.x = x;
    mouse->coords.y = y;
    window_position_set(mouse->graphic.window, x, y);
}

void mouse_click(struct mouse* mouse, MOUSE_CLICK_TYPE type)
{
    // Loop through every click handler and invoke it
    size_t total_click_handlers = vector_count(mouse->event_handlers.click_handlers);
    for(size_t i = 0; i < total_click_handlers; i++)
    {
        MOUSE_CLICK_EVENT_HANDLER_FUNCTION click_handler = NULL;
        vector_at(mouse->event_handlers.click_handlers, i, &click_handler, sizeof(click_handler));
        if (click_handler)
        {
            click_handler(mouse, mouse->coords.x, mouse->coords.y, type);
        }
    }
}

void mouse_moved(struct mouse* mouse)
{
    size_t total_move_handlers = vector_count(mouse->event_handlers.move_handlers);
    for(size_t i = 0; i < total_move_handlers; i++)
    {
        MOUSE_MOVE_EVENT_HANDLER_FUNCTION move_handler = NULL;
        vector_at(mouse->event_handlers.move_handlers, i, &move_handler, sizeof(move_handler));
        if(move_handler)
        {
            move_handler(mouse, mouse->coords.x, mouse->coords.y);
        }
    }
}

