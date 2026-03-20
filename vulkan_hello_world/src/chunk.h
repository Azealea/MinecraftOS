#pragma once

#include "block.h"
#include "stdint.h"

#define CHUNK_SIZE 32

#define CHUNK_NB_BLOCKS CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE

typedef struct Chunk
{
    int32_t x;
    int32_t y;
    int32_t z;

    Block blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
} Chunk;

Chunk* chunk_constr(int xpos, int ypos);

void chunk_deconstr(Chunk* c);

const Block* chunk_get(const Chunk* chunk, uint8_t x, uint8_t y, uint8_t z);

void chunk_set(Chunk* chunk, uint8_t x, uint8_t y, uint8_t z, Block block);

bool chunk_is_in_bound(uint8_t x, uint8_t y, uint8_t z);
