#pragma once

#include "utils/type.h"
#include "utils/vec.h"
#include "voxel/block.h"

#define CHUNK_SIZE 32

#define CHUNK_NB_BLOCKS CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE

typedef VEC3(i32) ChunkPos;

typedef struct Chunk
{
    Block blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
} Chunk;

Chunk* chunk_constr(void);

void chunk_deconstr(Chunk* c);

const Block* chunk_get(const Chunk* chunk, VEC3(u8) pos);

void chunk_set(Chunk* chunk, VEC3(u8) pos, Block block);

bool chunk_is_in_bound(VEC3(u8) relative_pos_chunk);

static inline VEC3(u8) pos_glob_to_rel(VEC3(i32) g)
{
    return VEC3_CAST(u8, VEC3_MOD(g, CHUNK_SIZE));
}

static inline VEC3(i32) pos_glob_to_chunk(VEC3(i32) g)
{
    return VEC3_DIV(g, CHUNK_SIZE);
}
