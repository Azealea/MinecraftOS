#pragma once

#include <stdint.h>

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

enum
{
    CONNECTED4_CASE_COUNT = 16,
    CONNECTED8_CASE_COUNT = 47,
};

uint8_t connected_mask_pack(bool u0, bool u1, bool v0, bool v1, bool nw, bool ne, bool sw,
                            bool se);

uint8_t connected8_case_index(uint8_t mask);
