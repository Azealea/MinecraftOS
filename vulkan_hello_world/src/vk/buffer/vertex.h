#pragma once

#include "app.h"

typedef struct Vertex
{
    float pos[2];
    float color[3];

    float texCoord[2];
} Vertex;

extern const Vertex vertices[];
uint32_t vertex_count(void);
uint32_t index_count(void);

void create_vertex_buffer(App* app);
void destroy_vertex_buffer(App* app);

void create_index_buffer(App* app);
void destroy_index_buffer(App* app);
