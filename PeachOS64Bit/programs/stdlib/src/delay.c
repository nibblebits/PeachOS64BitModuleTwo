#include "delay.h"
#include "peachos.h"

void udelay(uint64_t microseconds)
{
    peachos_udelay(microseconds);
}

void usleep(uint64_t miliseconds)
{
    udelay(miliseconds * 1000);
}