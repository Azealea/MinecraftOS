#pragma once

#include "camera.h"

typedef struct
{
    Camera cam;
} Player;

Player player_init(void);
