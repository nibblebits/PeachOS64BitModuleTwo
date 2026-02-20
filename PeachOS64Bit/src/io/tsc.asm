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

[BITS 64]
extern read_tsc
extern tsc_frequency
extern tsc_microseconds

; 128 bit types not supported by default in C
tsc_microseconds:
    push rbp
    mov rbp, rsp
    sub rsp, 32
    call tsc_frequency
    ; store the TSC frequency at rbp-8
    mov qword [rbp-8], rax
    ; store the current TSC at RBP-16
    call read_tsc
    mov qword [rbp-16], rax

    ; We now have TSC_frequency at rbp-8 and current_tsc at RBP-16
    mov rax, qword [rbp-16]
    mov rcx, 1000000
    ; current_tsc * 1000000
    mul rcx
    ; RDX:RAX = result
    ; RDX for overflow

    ; Divide by the tsc_freq
    ; RDX:RAX / RCX = (current_tsc * 1000000) / tsc_freq
    ; now we do the divide
    mov rcx, qword [rbp-8] ; Holds tsc_frequency
    div rcx
    ; RAX = microseconds

    ; Restore the stack
    add rsp, 32
    pop rbp
    ret

