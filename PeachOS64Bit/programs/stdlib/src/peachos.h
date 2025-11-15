#ifndef PEACHOS_H
#define PEACHOS_H
#include <stddef.h>
#include <stdbool.h>

// temporary: until we implement the GUI SDK 
struct window_event
{
    int type;
    int win_id;
    void* window;

    union 
    {
        struct
        {
            // empty no properties
        } focus;

        // positions are relative to the window body
        struct
        {
            int x;
            int y;
        } move;

        // relative to the window body.
        struct
        {
            int x;
            int y;
        } click;

    } data;
    
};

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

// Forward declare file stat.
struct file_stat;
struct window;


void print(const char* filename);
int peachos_getkey();

void* peachos_malloc(size_t size);
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

int peachos_fopen(const char* filename, const char* mode);
void peachos_fclose(size_t fd);
long peachos_fread(void* buffer, size_t size, size_t count, long fd);
long peachos_fseek(long fd, long offset, long whence);
long peachos_fstat(long fd, struct file_stat* file_stat_out);
void* peachos_realloc(void* old_ptr, size_t new_size);
void* peachos_window_create(const char* title, long width, long height, long flags, long id);
void peachos_divert_stdout_to_window(struct window* window);
int peachos_process_get_window_event(struct window_event* event);
void* peachos_window_get_graphics(struct window* window);
void* peachos_graphic_pixels_get(void* graphics);
void peachos_window_redraw(struct window* window);
#endif