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
    VECTOR_FREE(&c->entities);
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

Block* chunk_get(const Chunk* chunk, VEC3(u8) pos)
{
    return (Block*)&chunk->blocks[chunk_index(pos)];
}

VEC3(u8) chunk_pos_of(const Chunk* chunk, const Block* b)
{
    int idx = (int)(b - chunk->blocks);
    u8 y = (u8)(idx / (CHUNK_SIZE * CHUNK_SIZE));
    int rem = idx % (CHUNK_SIZE * CHUNK_SIZE);
    u8 z = (u8)(rem / CHUNK_SIZE);
    u8 x = (u8)(rem % CHUNK_SIZE);
    return (VEC3(u8)){x, y, z};
}

void chunk_trigger_update_around(Chunk* c, VEC3(u8) pos)
{
    for (int i = 0; i < 6; i++)
    {
        VEC3(u8) npos = VEC3_ADD(pos, BLOCK_NEIGHB_DIRS[i]);
        if (!chunk_is_in_bound(npos))
            continue;

        Block* neigh = &c->blocks[chunk_index(npos)];
        auto on_update_fn = blockBehaviours[neigh->type].on_update;
        if (on_update_fn)
        {
            on_update_fn(neigh, c);
        }
    }
}

void chunk_interact(Chunk* chunk, VEC3(u8) pos)
{
    Block* slot = &chunk->blocks[chunk_index(pos)];

    auto on_interact_fn = blockBehaviours[slot->type].on_interact;
    if (on_interact_fn)
    {
        on_interact_fn(slot, chunk);
    }
}

void chunk_set(Chunk* chunk, VEC3(u8) pos, Block block)
{
    Block* slot = &chunk->blocks[chunk_index(pos)];

    auto on_break_fn = blockBehaviours[slot->type].on_break;
    if (on_break_fn)
    {
        on_break_fn(slot, chunk);
    }

    *slot = block;
    auto on_place_fn = blockBehaviours[slot->type].on_place;
    if (on_place_fn)
    {
        on_place_fn(slot, chunk);
    }

    chunk_trigger_update_around(chunk, pos);
}
