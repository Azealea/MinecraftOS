#pragma once

#include "utils/container.h"
#include "voxel/bucket_alloc.h"
#include "voxel/chunk.h"
#include "voxel/face.h"

static inline size_t chunkpos_hash(ChunkPos p)
{
    uint64_t h = (uint64_t)(uint32_t)p.x * 73856093u ^ (uint64_t)(uint32_t)p.y * 19349663u
        ^ (uint64_t)(uint32_t)p.z * 83492791u;
    return (size_t)h;
}

static inline bool chunkpos_eq(ChunkPos a, ChunkPos b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

typedef Chunk* Chunk_ptr;

MAP_DECLARE(ChunkPos, Chunk_ptr, chunkpos_hash, chunkpos_eq)

typedef struct World
{
    MAP(ChunkPos, Chunk_ptr) chunks;
    BucketAlloc mesh;
} World;

World world_constr(void);
void world_free(World* w);

Chunk* world_get_chunk(World* w, ChunkPos pos);
Chunk* world_get_or_add_chunk(World* w, ChunkPos pos);

void generate_chunk_terrain(World* w, ChunkPos pos);

void world_queue_mesh_3by3by3(World* w, ChunkPos pos,
                              const FaceTexture (*block_faces)[FACE_COUNT]);
