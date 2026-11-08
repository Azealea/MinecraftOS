#pragma once

#include "utils/utils.h"

enum Direction : i8
{
    X_PLUS,
    X_MINUS,
    Y_PLUS,
    Y_MINUS,
    Z_PLUS,
    Z_MINUS,
    DIRECTION_COUNT,
};

enum Direction direction_opposite(enum Direction d);
enum Direction direction_nearest(VEC3(f32) v);

extern const VEC3(i8) DIRECTIONS[DIRECTION_COUNT];
