#pragma once

#include "utils/utils.h"

typedef struct Face
{
    VEC3(i32) pos;
    i32 face_id; // where to instantiate the face
    i32 texture_id;
} Face;
