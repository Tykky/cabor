#include "cabortime.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
double cabor_get_time() 
{
    LARGE_INTEGER frequency, counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / frequency.QuadPart;
}
#else
#include <time.h>
double cabor_get_time() 
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}
#endif
