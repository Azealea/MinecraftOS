#pragma once

#include "utils/type.h"
#include "utils/vec.h"
#include "voxel/block.h"
#include "voxel/mesh/face.h"

#define CHUNK_SIZE 16

#define CHUNK_NB_BLOCKS CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE

typedef VEC3(i32) ChunkPos;

typedef struct Chunk
{
    Block blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
    Face* bucket_ptr; // where mesh are stored in the Big Buffer (tm)
    uint32_t face_count;
    bool dirty; // needs meshing
} Chunk;

Chunk* chunk_constr(void);

void chunk_deconstr(Chunk* c);

const Block* chunk_get(const Chunk* chunk, VEC3(u8) pos);

void chunk_set(Chunk* chunk, VEC3(u8) pos, Block block);

bool chunk_is_in_bound(VEC3(u8) relative_pos_chunk);

static inline i32 floor_div_chunk(i32 v)
{
    i32 q = v / CHUNK_SIZE;
    i32 r = v % CHUNK_SIZE;
    return r < 0 ? q - 1 : q;
}

static inline u8 floor_mod_chunk(i32 v)
{
    i32 r = v % CHUNK_SIZE;
    return (u8)(r < 0 ? r + CHUNK_SIZE : r);
}

static inline VEC3(u8) pos_glob_to_rel(VEC3(i32) g)
{
    return VEC3_APPLY(u8, floor_mod_chunk, g);
}

static inline VEC3(i32) pos_glob_to_chunk(VEC3(i32) g)
{
    return VEC3_APPLY(i32, floor_div_chunk, g);
}
