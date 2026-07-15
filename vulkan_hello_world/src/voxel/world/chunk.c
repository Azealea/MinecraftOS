#include "chunk.h"

#include <stdlib.h>

Chunk* chunk_constr(void)
{
    Chunk* c = calloc(1, sizeof(Chunk));
    c->bucket_index = UINT32_MAX;
    return c;
}

void chunk_deconstr(Chunk* c)
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
    chunk->blocks[chunk_index(pos)] = block;
}
