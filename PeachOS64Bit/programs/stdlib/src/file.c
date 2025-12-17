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

#include "file.h"
#include "peachos.h"
int fopen(const char* filename, const char* mode)
{
    return (int) peachos_fopen(filename, mode);
}
void fclose(int fd)
{
    peachos_fclose((long) fd);
}

int fread(void* buffer, size_t size, size_t count, long fd)
{
    return peachos_fread(buffer, size, count, fd);
}

int fseek(int fd, int offset, int whence)
{
    return (int) peachos_fseek(fd, offset, whence);
}

int fstat(int fd, struct file_stat* file_stat_out)
{
    return (int) peachos_fstat(fd, file_stat_out);
}