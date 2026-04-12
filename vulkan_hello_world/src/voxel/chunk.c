#include "chunk.h"

#include <stdlib.h>

Chunk* chunk_constr(int xpos, int ypos)
{
    Chunk* c = calloc(1, sizeof(Chunk));
    c->x = xpos;
    c->y = ypos;
    return c;
}

void chunk_deconstr(Chunk* c)
{
    free(c);
}

bool chunk_is_in_bound(uint8_t x, uint8_t y, uint8_t z)
{
    return x < CHUNK_SIZE && y < CHUNK_SIZE && z < CHUNK_SIZE;
}

inline int chunk_index(uint8_t x, uint8_t y, uint8_t z)
{
    return x + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE;
}

const Block* chunk_get(const Chunk* chunk, uint8_t x, uint8_t y, uint8_t z)
{
    return &chunk->blocks[chunk_index(x, y, z)];
}

void chunk_set(Chunk* chunk, uint8_t x, uint8_t y, uint8_t z, Block block)
{
    chunk->blocks[chunk_index(x, y, z)] = block;
}
