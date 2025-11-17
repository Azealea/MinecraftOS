#pragma once

#include <signal.h>
#include <stdio.h>

#define ASSERT(ERROR, FORMAT, ...)                                             \
    do                                                                         \
    {                                                                          \
        if (!(ERROR))                                                          \
        {                                                                      \
            fprintf(stderr, "%s:%d -> %s :\n\t" FORMAT "\n", __FILE_NAME__,    \
                    __LINE__, __func__, ##__VA_ARGS__);                        \
            raise(SIGABRT);                                                    \
        }                                                                      \
    } while (0);

uint32_t clamp(uint32_t value, uint32_t min, uint32_t max);