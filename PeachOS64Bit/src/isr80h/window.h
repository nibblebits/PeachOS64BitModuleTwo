#ifndef ISR80H_WINDOW_H
#define ISR80H_WINDOW_H

struct interrupt_frame;
void* isr80h_command16_window_create(struct interrupt_frame* frame);
#endif