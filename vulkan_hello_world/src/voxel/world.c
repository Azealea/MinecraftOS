#include "world.h"

#include <stdlib.h>

#include "block.h"
#include "chunk.h"
#include "utils/container.h"
#include "voxel/mesh.h"

Chunk* world_get_chunk(World* w, ChunkPos p)
{
    Chunk_ptr* existing = MAP_GET_T(ChunkPos, Chunk_ptr, w->chunks, p);
    return existing ? *existing : NULL;
}

Chunk* world_get_or_add_chunk(World* w, ChunkPos p)
{
    Chunk_ptr* existing = MAP_GET_T(ChunkPos, Chunk_ptr, w->chunks, p);
    if (existing)
        return *existing;

    Chunk* c = chunk_constr();
    MAP_INSERT_T(ChunkPos, Chunk_ptr, w->chunks, p, c);
    return c;
}

void generate_chunk_terrain(World* w, ChunkPos pos)
{
    Chunk* c = world_get_or_add_chunk(w, pos);
    for (u8 x = 0; x < CHUNK_SIZE; x++)
        for (u8 y = 0; y < CHUNK_SIZE; y++)
            for (u8 z = 0; z < CHUNK_SIZE; z++)
            {
                BlockType t = rand() % 2 ? BLK_AIR
                                         : (BlockType)(1 + rand() % (BLOCK_COUNT - 1));
                chunk_set(c, (VEC3(u8)){x, y, z}, (Block){.type = t});
            }
}

void world_queue_mesh_3by3by3(World* w, ChunkPos pos,
                              const FaceTexture (*block_faces)[FACE_COUNT])
{
    for (int dx = -1; dx <= 1; dx++)
        for (int dy = -1; dy <= 1; dy++)
            for (int dz = -1; dz <= 1; dz++)
            {
                ChunkPos p = {pos.x + dx, pos.y + dy, pos.z + dz};
                Chunk* c = world_get_or_add_chunk(w, p);
                if (c->bucket_index == NO_BUCKET)
                    c->bucket_index = bucket_alloc_acquire(&w->mesh);
                c->face_count = generate_chunk_mesh(
                    c, p, bucket_ptr(&w->mesh, c->bucket_index), block_faces);
            }
}

World world_constr(void)
{
    World w = {
        .chunks = {0},
        .mesh = bucket_alloc_init(),
    };
    generate_chunk_terrain(&w, (ChunkPos){0, 0, 0});
    return w;
}

void world_free(World* w)
{
    MAP_FOR_EACH(w->chunks, entry)
    {
        chunk_deconstr(entry->value);
    }
    MAP_FREE(w->chunks);
    bucket_alloc_free(&w->mesh);
}
