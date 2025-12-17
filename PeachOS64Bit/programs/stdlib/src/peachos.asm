; Copyright (C) 2025 Daniel McCarthy <daniel@dragonzap.com>
; Part of the PeachOS Part Two Development Series.
; https://github.com/nibblebits/PeachOS64BitCourse
; https://github.com/nibblebits/PeachOS64BitModuleTwo
; Licensed under the GNU General Public License version 2 (GPLv2).
;
; Community contributors to this source file:
; NONE AS OF YET
; ----------------
; Disclaimer: Contributors are hobbyists that contributed to the public source code, they are not affiliated or endorsed by Daniel McCarthy the author of the PeachOS Kernel      
; development video series. Contributors did not contribute to the video content or the teaching and have no intellectual property rights over the video content for the course video * material and did not contribute to the video material in anyway.
; 
;

[BITS 64]

section .asm

global print:function
global peachos_getkey:function
global peachos_malloc:function
global peachos_free:function
global peachos_putchar:function
global peachos_process_load_start:function
global peachos_process_get_arguments:function 
global peachos_system:function
global peachos_exit:function
global peachos_fopen:function
global peachos_fclose:function
global peachos_fread:function
global peachos_fseek:function
global peachos_fstat:function
global peachos_realloc:function
global peachos_window_create:function
global peachos_divert_stdout_to_window:function
global peachos_process_get_window_event:function
global peachos_window_get_graphics:function
global peachos_window_redraw:function
global peachos_graphic_pixels_get:function
global peachos_graphics_create:function
global peachos_window_redraw_region:function
global peachos_window_title_set:function
global peachos_udelay:function;

; void print(const char* filename)
print:
    push qword rdi
    mov rax, 1 ; Command print
    int 0x80
    add rsp, 8
    ret

; int peachos_getkey()
peachos_getkey:
    mov rax, 2 ; Command getkey
    int 0x80
    ret

; void peachos_putchar(char c)
peachos_putchar:
    mov rax, 3 ; Command putchar
    push qword rdi ; Variable "c"
    int 0x80
    add rsp, 8
    ret

; void* peachos_malloc(size_t size)
peachos_malloc:
    mov rax, 4 ; Command malloc (Allocates memory for the process)
    push qword rdi ; Variable "size"
    int 0x80
    add rsp, 8
    ret

; void peachos_free(void* ptr)
peachos_free:
    mov rax, 5 ; Command 5 free (Frees the allocated memory for this process)
    push qword rdi ; Variable "ptr"
    int 0x80
    add rsp, 8
    ret

; void peachos_process_load_start(const char* filename)
peachos_process_load_start:
    mov rax, 6 ; Command 6 process load start ( stars a process )
    push qword rdi ; Variable "filename"
    int 0x80
    add rsp, 8
    ret

; int peachos_system(struct command_argument* arguments)
peachos_system:
    mov rax, 7 ; Command 7 process_system ( runs a system command based on the arguments)
    push qword rdi  ; Variable "arguments"
    int 0x80
    add rsp, 8
    ret


; void peachos_process_get_arguments(struct process_arguments* arguments)
peachos_process_get_arguments:
    mov rax, 8 ; Command 8 Gets the process arguments
    push qword rdi ; Variable arguments
    int 0x80
    add rsp, 8
    ret

; void peachos_exit()
peachos_exit:
    mov rax, 9 ; Command 9 process exit
    int 0x80
    ret

; int peachos_fopen(const char* filename, const char* mode)

peachos_fopen:
    mov rax, 10 ; Command 10, fopen
    push qword rsi   ; Pushes the mode 
    push qword rdi   ; Push the filename
    int 0x80        ; call the kernel
    add rsp, 16 ; restore the stack
    ret

; void peachos_fclose(size_t fd);
peachos_fclose:
    mov rax, 11 ; Command 11 fclose
    push qword rdi 
    int 0x80    ; call the kernel
    add rsp, 8  ; restore the stack
    ret

; long peachos_fread(void* buffer, size_t size, size_t count, long fd);
peachos_fread:
    mov rax, 12 ; Command 12 fread
    push qword rcx ; fd
    push qword rdx ; count
    push qword rsi ; size
    push qword rdi ; buffer
    int 0x80  ; invoke kernel
    add rsp, 32 ; restore the stack
    ret

; long peachos_fseek(long fd, long offset, long whence);
peachos_fseek:
    mov rax, 13 ; Command 13 fseek 
    push qword rdx ; whence
    push qword rsi ; offset
    push qword rdi ; fd
    int 0x80       ; invokes the kernel
    add rsp, 24    ; restores the stack
    ret            ; return

; long peachos_fstat(long fd, struct file_stat* file_stat_out)
peachos_fstat:
    mov rax, 14    ; Command 14 fstat
    push qword rsi  ; file_stat_out
    push qword rdi  ; fd
    int 0x80        ; call kernel
    add rsp, 16     ; restore stack
    ret

; void* peachos_realloc(void* old_ptr, size_t new_size);
peachos_realloc:
    mov rax, 15     ; Command 15 realloc
    push qword rsi  ; new_size
    push qword rdi  ; old_ptr
    int 0x80
    add rsp, 16
    ; RAX = new the pointer address
    ret

; void* peachos_window_create(const char* title, long width, long height, long flags, long id)
peachos_window_create:
    mov rax, 16
    push qword R8
    push qword rcx
    push qword rdx
    push qword rsi
    push qword rdi
    int 0x80
    ; restore the stack
    add rsp, 40

    ; RAX = contains the return result
    ret

; void peachos_divert_stdout_to_window(struct window* window);
peachos_divert_stdout_to_window:
    mov rax, 17  ; Command 17 - divert stdout to window
    push qword rdi  ; Pointer to userland window
    int 0x80
    add rsp, 8
    ret

; int peachos_process_get_window_event(struct window_event* event);
peachos_process_get_window_event:
    mov rax, 18 ; Command 18 get window event
    push qword rdi ; The pointer to the window event
    int 0x80
    add rsp, 8
    ; rax < 0 means error or no event
    ret

; void* peachos_window_get_graphics(struct window* window);
peachos_window_get_graphics:
    mov rax, 19 ; Command 19 get window graphics
    push qword rdi ; the pointer to the window
    int 0x80
    add rsp, 8

    ; RAX = struct userland_graphics*
    ret

; void* peachos_graphic_pixels_get(void* graphics);
peachos_graphic_pixels_get:
    mov rax, 20   ; Gets the pixel array pointer of a graphic entity
    push qword rdi ; push the graphics ptr.
    int 0x80
    add rsp, 8 
    ret

; void peachos_window_redraw(struct window* window);
peachos_window_redraw:
    mov rax, 21 ; Redraws the window
    push qword rdi ; push window pointer
    int 0x80
    add rsp, 8 
    ret

; void* peachos_graphics_create(size_t x, size_t y, size_t width, size_t height, void* parent_graphics);
peachos_graphics_create:
    mov rax, 22 ; COmmand 22 - create relative graphics
    push qword rdi ; x
    push qword rsi ; y
    push qword rdx ; width
    push qword rcx ; height
    push qword r8 ; parent graphics
    int 0x80
    add rsp, 40 ; restore the stack
    ; RAX = contain the new graphics metadata
    ret

; void peachos_window_redraw_region(long rel_x, long rel_y, long rel_width, long rel_height, struct window* window);
peachos_window_redraw_region:
    mov rax, 23 ; COmmand 23 redraw region on window
    push qword r8 ; window
    push qword rcx ; rel_height
    push qword rdx ; rel_width
    push qword rsi ; rel_y
    push qword rdi ; rel_x
    int 0x80
    add rsp, 40 ; restore stack
    ret

; void peachos_window_title_set(struct window* window, const char* title)
peachos_window_title_set:
    mov rax, 24  ; update window
    push qword rsi ; title
    push qword rdi ; window
    push qword 0 ; update type
    int 0x80 
    add rsp, 24 ; restore the stack
    ret

; void peachos_udelay(uint64_t microseconds);
peachos_udelay:
    mov rax, 25 ; command 25 udelay
    push qword rdi ; push the microseconds to delay by
    int 0x80        ; invoke the kernel
    add rsp, 8 ; restore stack
    ret
