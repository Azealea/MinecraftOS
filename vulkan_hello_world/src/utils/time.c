#define _POSIX_C_SOURCE 200809L
#include <time.h>

double current_time()
{
    struct timespec ts = {0};
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

double time_since(double start)
{
    return current_time() - start;
}

void sleep_seconds(double seconds)
{
    if (seconds <= 0.0)
        return;

    struct timespec ts = {
        .tv_sec = (time_t)seconds,
        .tv_nsec = (long)((seconds - (time_t)seconds) * 1e9),
    };
    nanosleep(&ts, nullptr);
}
