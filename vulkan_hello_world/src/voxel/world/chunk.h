#pragma once

#include "utils/vec.h"
#include "voxel/block.h"
#include "voxel/block/entities.h"
#include "voxel/mesh/face.h"

#define CHUNK_SIZE 16

#define CHUNK_NB_BLOCKS CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE

typedef VEC3(i32) ChunkPos;

typedef struct Chunk
{
    Block blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
    VECTOR(EntityData) entities;

    // runtime
    Face* bucket_ptr; // where mesh are stored in the Big Buffer (tm)
    uint32_t face_count;
    bool dirty; // needs meshing
} Chunk;

Chunk* chunk_alloc(void);
void chunk_free(Chunk* c);

Block* chunk_get(const Chunk* chunk, VEC3(u8) pos);
void chunk_set(Chunk* chunk, VEC3(u8) pos, Block block);
bool chunk_is_in_bound(VEC3(u8) relative_pos_chunk);
VEC3(u8) chunk_pos_of(const Chunk* chunk, const Block* b);

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

static inline ChunkPos pos_glob_to_chunk(VEC3(i32) g)
{
    return VEC3_APPLY(i32, floor_div_chunk, g);
}

EntityData* chunk_acquire_new_entity_slot(Chunk* c, u16* entity_id_ptr,
                                          enum BlockType btype);
EntityData* chunk_get_entity(Chunk* c, u16 entity_id);
void chunk_release_entity_slot(Chunk* c, u16 entity_id);

void chunk_trigger_update_around(Chunk* c, VEC3(u8) pos);
