#pragma once

#include <signal.h>
#include <stdint.h>

#include "utils/log.h"
#include "utils/vec.h"

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef double f64;
typedef float f32;

VEC3_DECLARE(f32);
VEC3_DECLARE(f64);

VEC3_DECLARE(i8);
VEC3_DECLARE(i16);
VEC3_DECLARE(i32);
VEC3_DECLARE(i64);
VEC3_DECLARE(u8);
VEC3_DECLARE(u16);
VEC3_DECLARE(u32);
VEC3_DECLARE(u64);

#define ERROR_EXIT(Format, ...)                                                          \
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
            ERROR_EXIT(Format, ##__VA_ARGS__)                                            \
        }                                                                                \
    } while (0);

#define ASSERTVK(Condition, Format, ...)                                                 \
    do                                                                                   \
    {                                                                                    \
        if ((Condition) != VK_SUCCESS)                                                   \
        {                                                                                \
            ERROR_EXIT(Format, ##__VA_ARGS__)                                            \
        }                                                                                \
    } while (0);

#define COUNTOF(arr) sizeof((arr)) / sizeof(*(arr))

uint32_t clamp(uint32_t value, uint32_t min, uint32_t max);
