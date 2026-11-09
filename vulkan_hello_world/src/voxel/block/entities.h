#pragma once

#include "utils/container.h"
#include "voxel/block/block.h"

typedef struct EntityData
{
    BlockType type;
} EntityData;

VECTOR_DECLARE(EntityData);
