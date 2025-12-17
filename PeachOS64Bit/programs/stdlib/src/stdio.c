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