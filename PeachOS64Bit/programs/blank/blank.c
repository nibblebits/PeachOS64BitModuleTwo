#include "peachos.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "file.h"
#include "delay.h"

struct window
{
    char title[128];
    int width;
    int height;
};

struct framebuffer_pixel
{
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t reserved;
};

struct userland_graphics
{
    size_t x;
    size_t y;
    size_t width;
    size_t height;

    // Pixels array
    void* pixels;

    // Pointer to the userland graphics pointer.
    void* userland_ptr;
};


int main(int argc, char** argv)
{
    struct window* win = peachos_window_create("Hello world", 200, 200, 0, 0);
    if (win)
    {
        printf("all okay\n");
    }

    // We want all printfs to go to the window
    peachos_divert_stdout_to_window(win);
    
    struct userland_graphics* graphics = peachos_window_get_graphics(win);
    if (!graphics)
    {
        printf("No graphics\n");
        return -1;
    }

    struct framebuffer_pixel* pixels = peachos_graphic_pixels_get(graphics);
    struct framebuffer_pixel blue = {.blue=0xff,.red=0x00,.green=0x00};
    for(size_t x = 0; x < graphics->width; x++)
    {
        for(size_t y = 0; y < graphics->height; y++)
        {
            pixels[y * graphics->width + x] = blue;
        }
    }

    for (int i = 0; i < 10; i++)
    {
        usleep(1000);
        printf("Iteration: %i\n", i);
    }
    
    peachos_window_redraw(win);
    while(1)
    {
        struct window_event window_event = {0};
        int res = peachos_process_get_window_event(&window_event);
        if (res >= 0)
        {
            printf("event type: %i\n", window_event.type);
        }
    }

    return 0;
}