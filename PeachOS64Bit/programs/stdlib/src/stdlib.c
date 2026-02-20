/*
 * PeachOS 64-Bit Kernel Project
 * Copyright (C) 2026 Daniel McCarthy <daniel@dragonzap.com>
 *
 * This file is part of the PeachOS 64-Bit Kernel.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <https://www.gnu.org/licenses/>.
 *
 * For full source code, documentation, and structured learning,
 * see the official kernel development course part one:
 * https://dragonzap.com/course/developing-a-multithreaded-kernel-from-scratch
 *
 * Get part one and part two module one, module two all peachos courses (69 hours of content): https://dragonzap.com/offer/kernel-development-from-scratch-69-hours
 *
 * Get the part two course module one and two: https://dragonzap.com/offer/developing-a-multithreaded-kernel-from-scratch-part-two-full-series
 */

/*
 * Copyright (C) 2025 Daniel McCarthy <daniel@dragonzap.com>
 * Part of the PeachOS Part Two Development Series.
 * https://github.com/nibblebits/PeachOS64BitCourse
 * https://github.com/nibblebits/PeachOS64BitModuleTwo
 * Licensed under the GNU General Public License version 2 (GPLv2).
 *
 * Community contributors to this source file:
 * NONE AS OF YET
 * ----------------
 * Disclaimer: Contributors are hobbyists that contributed to the public source code, they are not affiliated or endorsed by Daniel McCarthy the author of the PeachOS Kernel      
 * development video series. Contributors did not contribute to the video content or the teaching and have no intellectual property rights over the video content for the course video * material and did not contribute to the video material in anyway.
 */

#include "stdlib.h"
#include "peachos.h"
#include "memory.h"

/**
 * Incomplete atoi function
 * we ignore negation for exmaple among other things
 * very simple implementation incomplete but enough for what we need
 * atm.
 */
int atoi(const char* str)
{
    int result = 0;

    while(*str)
    {
        if (*str >= '0' && *str <= '9')
        {
            
            // NO wwe have a numerical value of the character
            int n_val = *str - '0';
            // base 10 should act as a shif tto the left
            // if im right..
            result = (result * 10) + n_val;
        }
        str++;
    }
    return result;
}
char* itoa(int i)
{
    static char text[12];
    int loc = 11;
    text[11] = 0;
    char neg = 1;
    if (i >= 0)
    {
        neg = 0;
        i = -i;
    }

    while(i)
    {
        text[--loc] = '0' - (i % 10);
        i /= 10;
    }

    if (loc == 11)
        text[--loc] = '0';
    
    if (neg)
        text[--loc] = '-';

    return &text[loc];
}

void* calloc(size_t n_memb, size_t size)
{
    size_t b_size = n_memb * size;
    void* ptr = malloc(b_size);
    if (!ptr)
    {
        return NULL;
    }
    
    memset(ptr, 0, b_size);
    return ptr;
}


 void* realloc(void* ptr, size_t new_size)
 {
    return peachos_realloc(ptr, new_size);
 }


void* malloc(size_t size)
{
    return peachos_malloc(size);
}

void free(void* ptr)
{
    peachos_free(ptr);
}