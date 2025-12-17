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

[BITS 64]
_start:

_start:
all:
    ;  very first instruction fails..
    ; must not be executable 
    mov rax, 3 ; Command putchar
    xor rdi, rdi
    mov di, 'A'
    push qword rdi
    int 0x80
    mov rcx, 0
.continue:
    inc rcx
    cmp rcx, 5000000
    jne .continue

    jmp all
    ret
