#pragma once

#include "app.h"
#include "voxel/mesh/face.h"
#include "voxel/world/bucket_alloc.h"

#define INSTANCE_MAX (BUCKET_COUNT * BUCKET_FACE_CAP)

VkVertexInputBindingDescription get_binding_description();
VkVertexInputAttributeDescription* get_attribute_descriptions(int* out_size);

void create_vertex_buffer(App* app);
void destroy_vertex_buffer(App* app);

void upload_chunk_mesh(App* app, const Face* faces, uint32_t face_count,
                       uint32_t bucket_index);
