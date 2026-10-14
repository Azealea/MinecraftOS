#include "chunk.h"

#include <stdlib.h>

#include "voxel/block.h"

Chunk* chunk_alloc(void)
{
    Chunk* c = calloc(1, sizeof(Chunk));
    return c;
}

void chunk_free(Chunk* c)
{
    free(c);
}

bool chunk_is_in_bound(VEC3(u8) pos)
{
    return pos.x < CHUNK_SIZE && pos.y < CHUNK_SIZE && pos.z < CHUNK_SIZE;
}

inline int chunk_index(VEC3(u8) pos)
{
    return pos.x + pos.z * CHUNK_SIZE + pos.y * CHUNK_SIZE * CHUNK_SIZE;
}

const Block* chunk_get(const Chunk* chunk, VEC3(u8) pos)
{
    return &chunk->blocks[chunk_index(pos)];
}

void chunk_set(Chunk* chunk, VEC3(u8) pos, Block block)
{
    Block old = chunk->blocks[chunk_index(pos)];
    auto on_break_fn = blockBehaviours[old.type].on_break;
    if (on_break_fn)
    {
        on_break_fn(&old, chunk);
    }

    auto on_place_fn = blockBehaviours[block.type].on_place;
    if (on_place_fn)
    {
        on_place_fn(&block, chunk);
    }

    chunk->blocks[chunk_index(pos)] = block;
}
