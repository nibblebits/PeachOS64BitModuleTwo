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

#include "stdio.h"
#include "peachos.h"
#include "stdlib.h"
#include "string.h"
#include <stdarg.h>
int putchar(int c)
{
    peachos_putchar((char)c);
    return 0;
}

int printf(const char *fmt, ...)
{
    va_list ap;
    const char *p;
    char *sval;
    int ival;

    va_start(ap, fmt);
    for (p = fmt; *p; p++)
    {
        if (*p != '%')
        {
            putchar(*p);
            continue;
        }

        switch (*++p)
        {
        case 'i':
            ival = va_arg(ap, int);
            print(itoa(ival));
            break;

        case 's':
            sval = va_arg(ap, char *);
            print(sval);
            break;

        default:
            putchar(*p);
            break;
        }
    }

    va_end(ap);

    return 0;
}

int sprintf(char *str, const char *fmt, ...)
{
    va_list ap;
    const char *p;
    char *sval;
    int ival;
    int size;

    va_start(ap, fmt);
    for (p = fmt; *p; p++)
    {
        if (*p != '%')
        {
            *str = *p;
            str++;
            continue;
        }

        switch (*++p)
        {
        case 'i':
            ival = va_arg(ap, int);
            strcpy(str, itoa(ival));
            size = strlen(itoa(ival));
            str+= size;
            break;

        case 's':
            sval = va_arg(ap, char *);
            strcpy(str, sval);
            size = strlen(sval);
            str+= size;
            break;

        default:
            *str = *p;
            str++;
            break;
        }
    }

    // Finally null terminator
    *str = 0x00;
    va_end(ap);

    return 0;
}