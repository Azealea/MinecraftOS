#pragma once

#include "app.h"
#include "voxel/mesh/face.h"
#include "voxel/world/chunk.h"

#define INSTANCE_MAX (BUCKET_COUNT * BUCKET_FACE_CAP)

VkVertexInputBindingDescription get_binding_description();
VkVertexInputAttributeDescription* get_attribute_descriptions(int* out_size);

void create_vertex_buffer(App* app);
void update_vertex_buffer(App* app, uint32_t bucket_index, uint32_t face_count);
void destroy_vertex_buffer(App* app);

void vk_rebuild_mesh(App* app, ChunkPos pos);
