#ifndef ISR80H_WINDOW_H
#define ISR80H_WINDOW_H

enum
{
    ISR80H_WINDOW_UPDATE_TITLE = 0
};
struct interrupt_frame;
void* isr80h_command16_window_create(struct interrupt_frame* frame);
void* isr80h_command17_sysout_to_window(struct interrupt_frame* frame);
void* isr80h_command18_get_window_event(struct interrupt_frame* frame);
void* isr80h_command19_window_graphics_get(struct interrupt_frame* frame);
void* isr80h_command21_window_redraw(struct interrupt_frame* frame);
void* isr80h_command23_window_redraw_region(struct interrupt_frame* frame);
void* isr80h_command24_update_window(struct interrupt_frame* frame);
#endif