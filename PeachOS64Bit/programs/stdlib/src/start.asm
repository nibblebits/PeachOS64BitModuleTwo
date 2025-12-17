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
extern c_start
extern peachos_exit

section .asm

_start:
    call c_start
    call peachos_exit
    ret