#include "window.h"
#include "graphics/window.h"
#include "graphics/graphics.h"
#include "task/task.h"
#include "task/process.h"
#include "task/userlandptr.h"

#include "status.h"
#include "kernel.h"

struct window* isr80h_window_from_process_window_virt(void* proc_win_virt_addr)
{
    struct process_window* proc_win = process_window_get_from_user_window(task_current()->process, proc_win_virt_addr);
    if (!proc_win)
    {
        return NULL;
    }

    struct window* kern_window = proc_win->kernel_win;
    if (!kern_window)
    {
        return NULL;
    }

    return kern_window;
}

void* isr80h_command16_window_create(struct interrupt_frame* frame)
{
    int res = 0;
    struct process_window* win = NULL;
    void* window_title_user_ptr = task_get_stack_item(task_current(), 0);
    char win_title[WINDOW_MAX_TITLE];
    res = copy_string_from_task(task_current(), window_title_user_ptr, win_title, sizeof(win_title));
    if (res < 0)
    {
        goto out;
    }

    int win_width = (int) (uintptr_t) task_get_stack_item(task_current(), 1);
    int win_height = (int) (uintptr_t) task_get_stack_item(task_current(), 2);
    int flags = (int) (uintptr_t) task_get_stack_item(task_current(), 3);
    int id = (int) (uintptr_t) task_get_stack_item(task_current(), 4);

    // Now lets create the window
    win = process_window_create(task_current()->process, win_title, win_width, win_height, flags, id);
    if (!win)
    {
        res = -EINVARG;
        goto out;
    }

out:
    if (res < 0)
    {
        if (win != NULL)
        {
            // free the window... todo
            win = NULL;
        }

        return NULL;
    }

    return win->user_win;
}
