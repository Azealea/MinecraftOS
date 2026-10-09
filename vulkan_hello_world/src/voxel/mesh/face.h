#pragma once

#include "utils/utils.h"

typedef struct Face
{
    VEC3(i32) pos;
    i32 face_id; // 0=+X, 1=-X, 2=+Y, 3=-Y, 4=+Z, 5=-Z
    i32 texture_id;
} Face;
