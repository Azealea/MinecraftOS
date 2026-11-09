#pragma once

#include "utils/utils.h"
#include "voxel/direction.h"

enum BlockOrientation : u8
{
    BLKORIENT_X_PLUS,
    BLKORIENT_X_MINUS,
    BLKORIENT_Y_PLUS,
    BLKORIENT_Y_MINUS,
    BLKORIENT_DOWN_X_PLUS,
    BLKORIENT_DOWN_X_MINUS,
    BLKORIENT_DOWN_Y_PLUS,
    BLKORIENT_DOWN_Y_MINUS,
    BLKORIENT_UP_X_PLUS,
    BLKORIENT_UP_X_MINUS,
    BLKORIENT_UP_Y_PLUS,
    BLKORIENT_UP_Y_MINUS,
    BLKORIENT_COUNT,
};

enum Direction block_orientation_facing(enum BlockOrientation o);
enum Direction block_orientation_up(enum BlockOrientation o);
