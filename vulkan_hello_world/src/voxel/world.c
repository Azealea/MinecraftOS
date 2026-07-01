#include "world.h"

#include <stdlib.h>

#include "block.h"
#include "chunk.h"
#include "utils/container.h"

#define WORLD_MESH_CAPACITY (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 6)

Chunk* world_get_or_add_chunk(World* w, ChunkPos p)
{
    Chunk_ptr* existing = MAP_GET_T(ChunkPos, Chunk_ptr, w->chunks, p);
    if (existing)
        return *existing;

    Chunk* c = chunk_constr();
    MAP_INSERT_T(ChunkPos, Chunk_ptr, w->chunks, p, c);
    return c;
}

static void FillFirstChunk(World* w)
{
    Chunk* tofill = world_get_or_add_chunk(w, ((ChunkPos){0}));
    for (int i = 0; i < 5; i++)
    {
        chunk_set(tofill, ((VEC3(u8)){i, 0, 0}), (Block){.type = i});
    }
}

World world_constr(void)
{
    World w = {
        .chunks = {0},
        .faces = malloc(WORLD_MESH_CAPACITY * sizeof(Face)),
        .face_count = 0,
    };

    FillFirstChunk(&w);

    return w;
}

void world_free(World* w)
{
    MAP_FOR_EACH(w->chunks, entry)
    {
        chunk_deconstr(entry->value);
    }
    MAP_FREE(w->chunks);
    free(w->faces);
}
