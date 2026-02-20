; PeachOS 64-Bit Kernel Project
; Copyright (C) 2026 Daniel McCarthy <daniel@dragonzap.com>
;
; This file is part of the PeachOS 64-Bit Kernel.
;
; This program is free software; you can redistribute it and/or
; modify it under the terms of the GNU General Public License
; version 2 as published by the Free Software Foundation.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
; GNU General Public License version 2 for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program; if not, see <https://www.gnu.org/licenses/>.
;
; For full source code, documentation, and structured learning,
; see the official kernel development course part one:
; https://dragonzap.com/course/developing-a-multithreaded-kernel-from-scratch
;
; Get part one and part two module one, module two all peachos courses (69 hours of content): https://dragonzap.com/offer/kernel-development-from-scratch-69-hours
;
; Get the part two course module one and two: https://dragonzap.com/offer/developing-a-multithreaded-kernel-from-scratch-part-two-full-series

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
global _start
_start:
all:
    ;  very first instruction fails..
    ; must not be executable 
    mov rax, 3 ; Command putchar
    xor rdi, rdi
    mov di, 'D'
    push qword rdi
    int 0x80
    mov rcx, 0
.continue:
    inc rcx
    cmp rcx, 5000000
    jne .continue

    jmp all
    ret
