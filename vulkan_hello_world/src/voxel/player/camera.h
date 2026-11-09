#pragma once

#include "input/input.h"
#include "utils/mat.h"
#include "utils/utils.h"

typedef struct CameraBasis
{
    VEC3(f32) front;
    VEC3(f32) right;
    VEC3(f32) up;
} CameraBasis;

typedef struct Camera
{
    VEC3(f32) pos;

    float yaw;
    float pitch;

    bool fast;

    bool firstMouse;

    CameraBasis basis;
} Camera;

Camera camera_init(VEC3(i32) pos);

void update_camera_pos(Camera* camera, const Input* input);
