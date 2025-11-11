#include "peachos.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "file.h"

struct window
{
    char title[128];
    int width;
    int height;
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