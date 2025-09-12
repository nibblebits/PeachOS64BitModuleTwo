#ifndef KERNEL_TSC_H
#define KERNEL_TSC_H
#include <stdint.h>
typedef uint64_t TIME_TSC;
typedef uint64_t TIME_MICROSECONDS;
typedef uint64_t TIME_MILISECONDS;
typedef uint64_t TIME_SECONDS;

TIME_TSC tsc_frequency(void);
void udelay(TIME_MICROSECONDS microseconds);
TIME_SECONDS tsc_seconds();
TIME_MILISECONDS tsc_miliseconds();
TIME_TSC read_tsc(void);

// tsc.asm 
TIME_MICROSECONDS tsc_microseconds();

#endif