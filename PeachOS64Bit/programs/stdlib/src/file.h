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

#ifndef USERLAND____FILE_H
#define USERLAND____FILE_H

#include <stddef.h>
#include <stdint.h>

typedef unsigned int FILE_STAT_FLAGS;
struct file_stat
{
    FILE_STAT_FLAGS flags;
    uint32_t filesize;
};

int fopen(const char* filename, const char* mode);
int fread(void* buffer, size_t size, size_t count, long fd);
void fclose(int fd);
int fseek(int fd, int offset, int whence);
int fstat(int fd, struct file_stat* file_stat_out);


#endif