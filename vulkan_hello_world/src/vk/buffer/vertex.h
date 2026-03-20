#pragma once

#include "app.h"

typedef struct Vertex
{
    int32_t pos[3];
    float texCoord[2];
    int32_t textureid;
} Vertex;

uint32_t vertex_count(void);
uint32_t index_count(void);

void recreate_vertices(const Chunk* chunk);

VkVertexInputBindingDescription get_binding_description();
VkVertexInputAttributeDescription* get_attribute_descriptions(int* out_size);

void create_vertex_buffer(App* app);
void destroy_vertex_buffer(App* app);

void create_index_buffer(App* app);
void destroy_index_buffer(App* app);
