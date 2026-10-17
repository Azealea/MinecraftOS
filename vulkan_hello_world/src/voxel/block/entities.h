#pragma once

#include "utils/container.h"
#include "voxel/block.h"

typedef struct EntityData
{
    BlockType type;
} EntityData;

VECTOR_DECLARE(EntityData);
