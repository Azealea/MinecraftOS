#pragma once

#include "chunk.h"
#include "utils/container.h"

static inline size_t chunkpos_hash(ChunkPos p)
{
    uint64_t h = (uint64_t)(uint32_t)p.x * 73856093u ^ (uint64_t)(uint32_t)p.y * 19349663u
        ^ (uint64_t)(uint32_t)p.z * 83492791u;
    return (size_t)h;
}

static inline bool chunkpos_eq(ChunkPos a, ChunkPos b)
{
    return VEC3_EQ(a, b);
}

typedef Chunk* Chunk_ptr;

MAP_DECLARE(ChunkPos, Chunk_ptr, chunkpos_hash, chunkpos_eq)
VECTOR_DECLARE(ChunkPos);

typedef struct World
{
    MAP(ChunkPos, Chunk_ptr) chunks;
} World;

World world_init(char* file);
void world_clean(World* w, char* file);

Chunk* world_get_chunk(World* w, ChunkPos pos);
Chunk* world_get_or_add_chunk(World* w, ChunkPos pos);

void world_set_block(World* w, VEC3(i32) global_pos, Block block,
                     VECTOR(ChunkPos)* dirty);
void world_mark_chunk_dirty(World* w, ChunkPos pos, VECTOR(ChunkPos)* dirty);

void generate_chunk_terrain(World* w, ChunkPos pos, VECTOR(ChunkPos)* dirty);
void world_dirty_existing_neighbors(World* w, ChunkPos pos, VECTOR(ChunkPos)* dirty);
