#include "world.h"

#include <stdlib.h>

#include "chunk.h"
#include "utils/container.h"
#include "voxel/block.h"

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

void world_mark_chunk_dirty(World* w, ChunkPos pos)
{
    Chunk* c = world_get_chunk(w, pos);
    if (!c || c->dirty)
        return;
    c->dirty = true;
    VECTOR_PUSH_BACK(w->dirty, pos);
}

void world_set_block(World* w, VEC3(i32) global_pos, Block block)
{
    ChunkPos cpos = pos_glob_to_chunk(global_pos);
    Chunk* c = world_get_or_add_chunk(w, cpos);
    chunk_set(c, pos_glob_to_rel(global_pos), block);
    world_mark_chunk_dirty(w, cpos);
}

void generate_chunk_terrain(World* w, ChunkPos pos)
{
    Chunk* c = world_get_or_add_chunk(w, pos);
    for (u8 x = 0; x < CHUNK_SIZE; x++)
        for (u8 y = 0; y < CHUNK_SIZE; y++)
            for (u8 z = 0; z < CHUNK_SIZE; z++)
            {
                BlockType t =
                    rand() % 2 ? BLK_AIR : (BlockType)(1 + rand() % (BLOCK_COUNT - 1));
                chunk_set(c, (VEC3(u8)){x, y, z}, (Block){.type = t});
            }
    world_mark_chunk_dirty(w, pos);
}

World world_constr(void)
{
    World w = {
        .chunks = {0},
        .mesh = bucket_alloc_init(),
        .dirty = {0},
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
    VECTOR_FREE(w->dirty);
}
