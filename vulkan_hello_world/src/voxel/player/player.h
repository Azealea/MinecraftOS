#pragma once

#include "camera.h"

typedef struct
{
    Camera cam;
} Player;

Player player_init(VEC3(i32) pos);
