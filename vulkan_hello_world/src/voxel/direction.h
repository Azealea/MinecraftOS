#pragma once

#include "utils/utils.h"

typedef enum Direction : i8
{
    X_PLUS,
    X_MINUS,
    Y_PLUS,
    Y_MINUS,
    Z_PLUS,
    Z_MINUS,
    DIRECTION_COUNT,
} Direction;

Direction direction_opposite(Direction d);

extern const VEC3(i8) DIRECTIONS[DIRECTION_COUNT];
