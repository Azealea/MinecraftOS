#pragma once

#include "vk/camera.h"
#include "voxel/world.h"

typedef struct RaycastHit
{
    bool hit;
    VEC3(u32) b;
    VEC3(u32) n;
} RaycastHit;

RaycastHit raycast(const Camera* cam, World* world, float max_dist);
