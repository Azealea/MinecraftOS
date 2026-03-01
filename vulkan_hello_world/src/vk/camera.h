#pragma once

#include <cglm/mat4.h>

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

    double lastX;
    double lastY;
    bool firstMouse;

    CameraBasis basis;
} Camera;

Camera camera_init(void);
