#pragma once

#include "camera.h"
#include "voxel/block.h"

typedef struct
{
    Camera cam;
    enum BlockType chosenBlock;
} Player;

Player player_init(VEC3(i32) pos);
