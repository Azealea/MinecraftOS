#pragma once

#include <cglm/mat4.h>

#include "input.h"

typedef struct CameraBasis
{
    vec3 front;
    vec3 right;
    vec3 up;
} CameraBasis;

typedef struct Camera
{
    vec3 pos;

    float yaw;
    float pitch;

    bool fast;

    bool firstMouse;

    CameraBasis basis;
} Camera;

Camera camera_init(void);

void update_camera_pos(Camera* camera, const Input* input);
