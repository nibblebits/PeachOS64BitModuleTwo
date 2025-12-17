#include "kernel.h"
#include <stddef.h>
#include <stdint.h>
#include "idt/idt.h"
#include "memory/heap/kheap.h"
#include "memory/heap/heap.h"
#include "memory/paging/paging.h"
#include "memory/memory.h"
#include "keyboard/keyboard.h"
#include "mouse/mouse.h"
#include "string/string.h"
#include "isr80h/isr80h.h"
#include "io/tsc.h"
#include "io/pci.h"
#include "task/task.h"
#include "task/process.h"
#include "graphics/font.h"
#include "graphics/terminal.h"
#include "graphics/window.h"
#include "fs/file.h"
#include "disk/disk.h"
#include "disk/gpt.h"
#include "fs/pparser.h"
#include "disk/streamer.h"
#include "task/tss.h"
#include "gdt/gdt.h"
#include "graphics/graphics.h"
#include "graphics/image/image.h"
#include "config.h"
#include "status.h"

struct terminal* system_terminal = NULL;
void terminal_writechar(char c, char colour)
{
   if (!system_terminal)
   {
     return;
   }

   terminal_write(system_terminal, c);
}
void print(const char *str)
{
    size_t len = strlen(str);
    for (int i = 0; i < len; i++)
    {
        terminal_writechar(str[i], 15);
    }
}

void panic(const char *msg)
{
    print(msg);
    while (1)
    {
    }
}

// static struct paging_4gb_chunk* kernel_chunk = 0;

// void kernel_page()
// {
//     kernel_registers();
//     paging_switch(kernel_chunk);
// }

// struct tss tss;
// struct gdt gdt_real[PEACHOS_TOTAL_GDT_SEGMENTS];
// struct gdt_structured gdt_structured[PEACHOS_TOTAL_GDT_SEGMENTS] = {
//     {.base = 0x00, .limit = 0x00, .type = 0x00},                // NULL Segment
//     {.base = 0x00, .limit = 0xffffffff, .type = 0x9a},           // Kernel code segment
//     {.base = 0x00, .limit = 0xffffffff, .type = 0x92},            // Kernel data segment
//     {.base = 0x00, .limit = 0xffffffff, .type = 0xf8},              // User code segment
//     {.base = 0x00, .limit = 0xffffffff, .type = 0xf2},             // User data segment
//     {.base = (uint32_t)&tss, .limit=sizeof(tss), .type = 0xE9}      // TSS Segment
// };

struct tss tss;

extern struct gdt_entry gdt[];

// page descriptor
struct paging_desc *kernel_paging_desc = 0;

void kernel_page()
{
    kernel_registers();
    paging_switch(kernel_paging_desc);
}


struct paging_desc* kernel_desc()
{
    return kernel_paging_desc;
}

// defined in kernel.asm
extern struct graphics_info default_graphics_info;
void kernel_main()
{
    struct graphics_info* screen_info = NULL;

    print("Hello 64-bit!\n");

    print("Total memory\n");
    print(itoa(e820_total_accessible_memory()));
    print("\n");

    kheap_init();

    char *data = kmalloc(50);
    data[0] = 'A';
    data[1] = 'B';
    data[2] = 'C';
    data[3] = 0x00;
    print(data);
    kernel_paging_desc = paging_desc_new(PAGING_MAP_LEVEL_4);
    if (!kernel_paging_desc)
    {
        panic("Failed to create kernel paging descriptor\n");
    }
    paging_map_e820_memory_regions(kernel_paging_desc);

    paging_switch(kernel_paging_desc);

    // The multi-heap is ready
    kheap_post_paging();

    // Setup the graphics
    graphics_setup(&default_graphics_info);

    screen_info = graphics_screen_info();

    // Enable interrupt descriptor table
    idt_init();

    // Enable PCI and scan for devices
    pci_init();

    // Enable fs functionality
    fs_init();

    // Enable the disks
    disk_search_and_init();

    // Initialize GPT(gloabl partition table) drives
    gpt_init();

    // Initialize the font system
    font_system_init();

    // Setup the terminal system
    terminal_system_setup();

    // Initialize mouse system
    mouse_system_init();

    // Initialize the keyboard
    keyboard_init();

    // Initialize window system
    window_system_initialize();

    // Load the static mouse drivers.
    mouse_system_load_static_drivers();

    // initialize stage two graphics setup
    graphics_setup_stage_two(&default_graphics_info);

    // in no particular order.
    // initialize graphics stage two
    // initialize mouse system
    // initialize keyboard system

    // Initialize window system stage two
    window_system_initialize_stage2();

    struct font* font = font_get_system_font();
    if (!font)
    {
        panic("Failed to load system font\n");
    }

    struct framebuffer_pixel font_color = {0};
    font_color.red = 0xff;

    system_terminal = terminal_create(screen_info, 0, 0, screen_info->width, screen_info->height, font, font_color, TERMINAL_FLAG_BACKSPACE_ALLOWED);
    if (!system_terminal)
    {
        panic("Failed to create system terminal\n");
    }
    // Allocate a 1 MB stack for the kernel IDT 
    size_t stack_size = 1024*1024;
    void* megabyte_stack_tss_end = kzalloc(stack_size);
    void* megabyte_stack_tss_begin = (void*)(((uintptr_t) megabyte_stack_tss_end) + stack_size);
    
    // Block the first page
    paging_map(kernel_desc(), megabyte_stack_tss_end, megabyte_stack_tss_end, 0);

    // Setup the TSS
    memset(&tss, 0x00, sizeof(tss));
    tss.rsp0 = (uint64_t) megabyte_stack_tss_begin;
    tss.iopb_offset = sizeof(tss); // No I/O permissions are used

    struct tss_desc_64* tssdesc = (struct tss_desc_64*)&gdt[KERNEL_LONG_MODE_TSS_GDT_INDEX];
    gdt_set_tss(tssdesc, &tss, sizeof(tss)-1, TSS_DESCRIPTOR_TYPE, 0x00);
    
    // load the tss
    tss_load(KERNEL_LONG_MODE_TSS_SELECTOR);

    // Initialize the process system
    process_system_init();

    print("tss load was fine\n");
    // Register isr80h commands
    isr80h_register_commands();
    print("register isr80h\n");



    // struct image* img = graphics_image_load("@:/bkground.bmp");
    // graphics_draw_image(NULL, img, 0, 0);
    // graphics_redraw_all();

   

   //enable_interrupts();

    print("Total PCI devices:");
    print(itoa((int)pci_device_count()));
    print("\n");
   
    print("Loading program...\n");
    struct process* process = 0;
    int res = process_load_switch("@:/shell.elf", &process);
    if (res != PEACHOS_ALL_OK)
    {
        panic("Failed to load user program\n");
    }

    // Drop to user land
    task_run_first_ever_task();

   

    while (1)
    {
    }
}