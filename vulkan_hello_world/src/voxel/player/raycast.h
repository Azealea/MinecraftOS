#pragma once

#include "vk/camera.h"
#include "voxel/world/world.h"

typedef struct RaycastHit
{
    bool hit;
    VEC3(i32) block;
    VEC3(i32) normal;
} RaycastHit;

RaycastHit raycast(const Camera* cam, World* world, float max_dist);
