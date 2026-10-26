#include "mesh.h"

#include "voxel/block_state.h"
#include "voxel/textures/connected_texture.h"
#include "voxel/textures/texture_flags.h"

// offset for uvs passed down to vertex
typedef struct
{
    VEC3(i8) u0, u1, v0, v1;
} FaceEdgeDirs;

// TODO : move block_at function into world.c or chunk.c
static const Block* block_at(const Chunk* chunk, ChunkPos chunk_pos, World* world,
                             VEC3(i32) chunk_relative)
{
    if (chunk_relative.x >= 0 && chunk_relative.x < CHUNK_SIZE && chunk_relative.y >= 0
        && chunk_relative.y < CHUNK_SIZE && chunk_relative.z >= 0
        && chunk_relative.z < CHUNK_SIZE)
        return chunk_get(chunk, VEC3_CAST(u8, chunk_relative));

    VEC3(i32) global = VEC3_ADD(VEC3_SCALE(chunk_pos, CHUNK_SIZE), chunk_relative);
    Chunk* nc = world_get_chunk(world, pos_glob_to_chunk(global));
    return nc ? chunk_get(nc, pos_glob_to_rel(global)) : NULL;
}

static bool same_type_at(const Chunk* chunk, ChunkPos pos, World* world,
                         VEC3(i32) chunk_relative, BlockType type)
{
    const Block* nb = block_at(chunk, pos, world, chunk_relative);
    return nb && nb->type == type;
}

static u8 connected_mask_pack(bool u0, bool u1, bool v0, bool v1, bool nw, bool ne,
                              bool sw, bool se)
{
    u8 mask = 0;
    if (u0)
        mask |= CONNECTED4_U0;
    if (u1)
        mask |= CONNECTED4_U1;
    if (v0)
        mask |= CONNECTED4_V0;
    if (v1)
        mask |= CONNECTED4_V1;
    if (nw)
        mask |= CONNECTED8_NW;
    if (ne)
        mask |= CONNECTED8_NE;
    if (sw)
        mask |= CONNECTED8_SW;
    if (se)
        mask |= CONNECTED8_SE;
    return mask;
}

static u8 connected_neighbor_mask(const Chunk* chunk, ChunkPos pos, World* world,
                                  VEC3(i32) local, Direction face, BlockType type,
                                  u8 flags)
{
    static const FaceEdgeDirs face_edge_dirs[DIRECTION_COUNT] = {
        [X_PLUS] = {.u0 = {0, -1, 0}, .u1 = {0, 1, 0}, .v0 = {0, 0, 1}, .v1 = {0, 0, -1}},
        [X_MINUS] = {.u0 = {0, 1, 0},
                     .u1 = {0, -1, 0},
                     .v0 = {0, 0, 1},
                     .v1 = {0, 0, -1}},
        [Y_PLUS] = {.u0 = {1, 0, 0}, .u1 = {-1, 0, 0}, .v0 = {0, 0, 1}, .v1 = {0, 0, -1}},
        [Y_MINUS] = {.u0 = {-1, 0, 0},
                     .u1 = {1, 0, 0},
                     .v0 = {0, 0, 1},
                     .v1 = {0, 0, -1}},
        [Z_PLUS] = {.u0 = {0, 1, 0}, .u1 = {0, -1, 0}, .v0 = {1, 0, 0}, .v1 = {-1, 0, 0}},
        [Z_MINUS] = {.u0 = {0, -1, 0},
                     .u1 = {0, 1, 0},
                     .v0 = {1, 0, 0},
                     .v1 = {-1, 0, 0}},
    };
    if (!(flags & (BLKTXT_CONNECTED4_BIT | BLKTXT_CONNECTED8_BIT)))
        return 0;

    const FaceEdgeDirs* d = &face_edge_dirs[face];
    bool u0 = same_type_at(chunk, pos, world, VEC3_ADD(local, d->u0), type);
    bool u1 = same_type_at(chunk, pos, world, VEC3_ADD(local, d->u1), type);
    bool v0 = same_type_at(chunk, pos, world, VEC3_ADD(local, d->v0), type);
    bool v1 = same_type_at(chunk, pos, world, VEC3_ADD(local, d->v1), type);

    if (!(flags & BLKTXT_CONNECTED8_BIT))
        return connected_mask_pack(u0, u1, v0, v1, false, false, false, false);

    bool nw =
        same_type_at(chunk, pos, world, VEC3_ADD(local, VEC3_ADD(d->u0, d->v0)), type);
    bool ne =
        same_type_at(chunk, pos, world, VEC3_ADD(local, VEC3_ADD(d->u1, d->v0)), type);
    bool sw =
        same_type_at(chunk, pos, world, VEC3_ADD(local, VEC3_ADD(d->u0, d->v1)), type);
    bool se =
        same_type_at(chunk, pos, world, VEC3_ADD(local, VEC3_ADD(d->u1, d->v1)), type);
    return connected_mask_pack(u0, u1, v0, v1, nw, ne, sw, se);
}

static inline u32 hash3(VEC3(u8) pos)
{
    u32 h = pos.x * 0x9e3779b1u;
    h = (h ^ pos.y) * 0x85ebca6bu;
    h = (h ^ pos.z) * 0xc2b2ae35u;
    return h ^ (h >> 16);
}

u32 generate_chunk_mesh(const Chunk* chunk, ChunkPos pos, World* world,
                        const FaceTexture block_faces[][6])
{
    u32 count = 0;

    for (u8 x = 0; x < CHUNK_SIZE; x++)
        for (u8 y = 0; y < CHUNK_SIZE; y++)
            for (u8 z = 0; z < CHUNK_SIZE; z++)
            {
                VEC3(u8) local = {x, y, z};
                const Block* b = chunk_get(chunk, local);
                if (b->type == BLK_AIR)
                    continue;

                // bool rotates = blockBehaviours[b->type].rotates;

                for (Direction f = 0; f < DIRECTION_COUNT; f++)
                {
                    VEC3(u8) neighbor = VEC3_ADD(local, DIRECTIONS[f]);

                    if (chunk_is_in_bound(neighbor)
                        && chunk_get(chunk, neighbor)->type != BLK_AIR)
                        continue;

                    // u16 rotation = block_state_get(b->state, BLK_STATE_ROTATION_BIT);
                    // int tex_face = rotates ? get_rotated_index(f, rotation) : f;
                    const FaceTexture* ft = &block_faces[b->type][f];
                    u8 neighbor_mask = connected_neighbor_mask(
                        chunk, pos, world, VEC3_CAST(i32, local), f, b->type, ft->flags);

                    chunk->bucket_ptr[count++] = (Face){
                        .pos = VEC3_ADD(VEC3_SCALE(pos, CHUNK_SIZE), local),
                        .face_id = f,
                        .texture_id = face_texture_to_index(
                            ft, neighbor_mask,
                            block_state_get(b->state, BLK_STATE_ACTIVATED_BIT),
                            hash3(local)),
                    };
                }
            }

    return count;
}
