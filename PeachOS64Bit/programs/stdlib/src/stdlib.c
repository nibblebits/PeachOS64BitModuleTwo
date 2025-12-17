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