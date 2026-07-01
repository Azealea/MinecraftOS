#pragma once

#include <stdint.h>

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
