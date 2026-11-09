#pragma once

#include "app.h"
#include "utils/mat.h"

struct UniformBufferObject
{
    mat4 model;
    mat4 view;
    mat4 proj;
};

void create_uniform_buffers(App* app);
void destroy_uniform_buffers(App* app);
void update_uniform_buffer(App* app, uint32_t currentImage);
