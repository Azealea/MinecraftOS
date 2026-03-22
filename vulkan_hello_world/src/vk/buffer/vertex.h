#pragma once

#include "app.h"

typedef struct Face
{
    int32_t pos[3];
    int32_t face_id; // 0=+X, 1=-X, 2=+Y, 3=-Y, 4=+Z, 5=-Z
    int32_t texture_id;
} Face;

uint32_t instance_count(void);

void recreate_vertices(const Chunk* chunk);

VkVertexInputBindingDescription get_binding_description();
VkVertexInputAttributeDescription* get_attribute_descriptions(int* out_size);

void create_vertex_buffer(App* app);
void destroy_vertex_buffer(App* app);
