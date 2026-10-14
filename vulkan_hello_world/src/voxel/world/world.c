#include "world.h"

#include <stdio.h>
#include <stdlib.h>

#include "chunk.h"
#include "utils/container.h"
#include "utils/log.h"
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

    Chunk* c = chunk_alloc();
    MAP_INSERT_T(ChunkPos, Chunk_ptr, w->chunks, p, c);
    return c;
}

void world_mark_chunk_dirty(World* w, ChunkPos pos, VECTOR(ChunkPos)* dirty)
{
    Chunk* c = world_get_chunk(w, pos);
    if (!c || c->dirty)
        return;
    c->dirty = true;
    VECTOR_PUSH_BACK(dirty, pos);
}

void world_set_block(World* w, VEC3(i32) global_pos, Block block, VECTOR(ChunkPos)* dirty)
{
    ChunkPos cpos = pos_glob_to_chunk(global_pos);
    Chunk* c = world_get_or_add_chunk(w, cpos);
    chunk_set(c, pos_glob_to_rel(global_pos), block);
    world_mark_chunk_dirty(w, cpos, dirty);
}

void generate_chunk_terrain(World* w, ChunkPos pos, VECTOR(ChunkPos)* dirty)
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
    world_mark_chunk_dirty(w, pos, dirty);
}

// update connected texture (and mesh ) // TODO do a function to update texture only ?
void world_dirty_existing_neighbors(World* w, ChunkPos pos, VECTOR(ChunkPos)* dirty)
{
    static const ChunkPos ADJ[6] = {
        {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1},
    };
    for (int i = 0; i < 6; i++)
    {
        ChunkPos npos = VEC3_ADD(pos, ADJ[i]);
        if (world_get_chunk(w, npos))
            world_mark_chunk_dirty(w, npos, dirty);
    }
}

void world_save(const World* w, char* save_to)
{
    FILE* f = fopen(save_to, "w");
    ASSERT(f, "failed to open");

    size_t nb_chunks = MAP_SIZE(w->chunks);

    fwrite(&nb_chunks, sizeof(nb_chunks), 1, f);

    MAP_FOR_EACH(w->chunks, chunk_pair)
    {
        ChunkPos k = chunk_pair->key;
        const Chunk* c = chunk_pair->value;
        fwrite(&k, sizeof(k), 1, f);
        fwrite(c->blocks, sizeof(c->blocks), 1, f);
    }

    ASSERT(fclose(f) == 0, "failed to close file for save");
    INFO("saved world");
}

static World world_load(char* load_from)
{
    World res = {0};

    FILE* f = fopen(load_from, "r");
    ASSERT(f, "failed to open");

    size_t nb_chunks = 0;
    ASSERT(fread(&nb_chunks, sizeof(nb_chunks), 1, f) == 1, "failed read");

    for (size_t i = 0; i < nb_chunks; i++)
    {
        ChunkPos pos;
        ASSERT(fread(&pos, sizeof(pos), 1, f) == 1, "failed read");

        Chunk* c = world_get_or_add_chunk(&res, pos);

        ASSERT(fread(c->blocks, sizeof(c->blocks), 1, f) == 1, "failed read");
    }

    ASSERT(fclose(f) == 0, "failed to close file");
    INFO("loaded world");
    return res;
}

World world_init(char* load_from)
{
    if (load_from)
    {
        return world_load(load_from);
    }

    World w = {.chunks = {0}};

    return w;
}

void world_clean(World* w, char* save_to)
{
    if (save_to)
        world_save(w, save_to);
    MAP_FOR_EACH(w->chunks, entry)
    {
        chunk_free(entry->value);
    }
    MAP_FREE(w->chunks);
}
