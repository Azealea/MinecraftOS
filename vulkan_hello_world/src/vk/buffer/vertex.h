#pragma once

#include "app.h"
#include "voxel/chunk.h"
#include "voxel/face.h"

#define INSTANCE_MAX (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 6)

VkVertexInputBindingDescription get_binding_description();
VkVertexInputAttributeDescription* get_attribute_descriptions(int* out_size);

void create_vertex_buffer(App* app, const Face* faces, uint32_t count);
void destroy_vertex_buffer(App* app);
