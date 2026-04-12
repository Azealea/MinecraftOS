#pragma once

#include "voxel/chunk.h"

typedef struct World
{
    Chunk* chunk;
} World;

World world_constr(void);
void world_free(World* w);
