#pragma once

#include "utils/utils.h"

enum
{
    CONNECTED4_U0 = 1 << 0,
    CONNECTED4_U1 = 1 << 1,
    CONNECTED4_V0 = 1 << 2,
    CONNECTED4_V1 = 1 << 3,
};

enum
{
    CONNECTED8_NW = 1 << 4,
    CONNECTED8_NE = 1 << 5,
    CONNECTED8_SW = 1 << 6,
    CONNECTED8_SE = 1 << 7,
};

#define CONNECTED4_CASE_COUNT 16
#define CONNECTED8_CASE_COUNT 47

u8 connected8_case_index(u8 mask);
