#include "tsc.h"
#include "cpuid.h"

uint64_t tsc_freq_val = 0;
TIME_TSC tsc_frequency(void)
{
    if (tsc_freq_val != 0)
    {
        return tsc_freq_val;
    }

    uint32_t eax, ebx, ecx, edx;
    uint64_t tsc_freq = 0;

    // 0x15 first
    cpuid(0x15, 0, &eax, &ebx, &ecx, &edx);
    if (eax != 0 && ebx != 0 && ecx != 0)
    {
        tsc_freq = ((uint64_t) ecx * (uint64_t) ebx) / (uint64_t) eax;
    }

    // fallback: leaf 0x16
    if (tsc_freq == 0)
    {
        cpuid(0x16, 0, &eax, &ebx, &ecx, &edx);
        if (eax != 0)
        {
            // CPUID 0x16  : EAX gives the base frequency in Mhz.
            tsc_freq = (uint64_t) eax * 1000000ULL;
        }
    }

    // Multiply by 1,000,000 to get hertz
    tsc_freq *= 1000000;
    tsc_freq_val = tsc_freq;

    return (TIME_TSC) tsc_freq; 
}

TIME_TSC read_tsc(void)
{
    uint32_t lo, hi;
    __asm__ volatile ("lfence; rdtsc" : "=a"(lo), "=d"(hi) :: "memory");
    return ((TIME_TSC) hi << 32) | lo;
}

TIME_MILISECONDS tsc_miliseconds()
{
    TIME_MICROSECONDS microseconds = tsc_microseconds();
    return microseconds / 1000;
}

TIME_SECONDS tsc_seconds()
{
    TIME_MILISECONDS miliseconds = tsc_miliseconds();
    return miliseconds / 1000;
}

void udelay(TIME_MICROSECONDS microseconds)
{
    TIME_TSC tsc_freq = tsc_frequency();
    TIME_TSC start = read_tsc();
    TIME_TSC cycles_to_wait = (microseconds * tsc_freq) / 1000000;
    while((read_tsc() - start) < cycles_to_wait)
    {
        __asm__ volatile ("pause");
    }
}