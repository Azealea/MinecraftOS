#define _POSIX_C_SOURCE 200809L
#include <time.h>

double current_time()
{
    struct timespec ts = { 0 };
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

double time_since(double start)
{
    return current_time() - start;
}
