#pragma once

#include "utils/utils.h"

enum BlockState : u16
{
    BLK_STATE_ACTIVATED_BIT = 0b1,
    BLK_STATE_ROTATION_BIT = 0b11110,
};

static inline u16 block_state_get(u16 state, enum BlockState option)
{
    int offset = __builtin_ctz(option & -option);
    return (state & option) >> offset;
}

static inline void block_state_set(u16* state, enum BlockState option, u16 value)
{
    int offset = __builtin_ctz(option & -option);
    *state = (~option & *state) | (value << offset);
}
