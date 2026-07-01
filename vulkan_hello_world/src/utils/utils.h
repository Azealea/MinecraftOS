#pragma once

#include <signal.h>
#include <stdint.h>
#include <stdio.h>

#include "type.h"

#define ERROR(Format, ...)                                                               \
    do                                                                                   \
    {                                                                                    \
        fprintf(stderr, "%s:%d -> %s :\n\t" Format "\n", __FILE_NAME__, __LINE__,        \
                __func__, ##__VA_ARGS__);                                                \
        raise(SIGABRT);                                                                  \
    } while (0);

#define ASSERT(Condition, Format, ...)                                                   \
    do                                                                                   \
    {                                                                                    \
        if (!(Condition))                                                                \
        {                                                                                \
            ERROR(Format, ##__VA_ARGS__)                                                 \
        }                                                                                \
    } while (0);

#define ASSERTVK(Condition, Format, ...)                                                 \
    do                                                                                   \
    {                                                                                    \
        if ((Condition) != VK_SUCCESS)                                                   \
        {                                                                                \
            ERROR(Format, ##__VA_ARGS__)                                                 \
        }                                                                                \
    } while (0);

#define COUNTOF(arr) sizeof((arr)) / sizeof(*(arr))

uint32_t clamp(uint32_t value, uint32_t min, uint32_t max);
