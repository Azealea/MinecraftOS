#pragma once

#include "utils/utils.h"

enum textureFlags : u8
{
    BLKTXT_CONNECTED4_BIT = 1 << 0,
    BLKTXT_CONNECTED8_BIT = 1 << 1,
    BLKTXT_ACTIVATED_BIT = 1 << 2,
    BLKTXT_VARIANT_BIT = 1 << 3,
};
