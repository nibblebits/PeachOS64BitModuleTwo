#ifndef KERNEL_ISR80H_GRAPHICS_H
#define KERNEL_ISR80H_GRAPHICS_H

#include <stdint.h>
#include <stddef.h>

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

struct userland_graphics* isr80h_graphics_make_userland_metadata(struct process* process, struct graphics_info* graphics_info);
void* isr80h_command20_graphics_pixels_get(struct interrupt_frame* frame);

#endif