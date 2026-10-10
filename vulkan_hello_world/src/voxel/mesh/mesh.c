#include "mesh.h"

#include "utils/utils.h"
#include "voxel/textures/connected_texture.h"
#include "voxel/textures/resolve/face_texture.h"
#include "voxel/textures/texture_flags.h"
#include "voxel/world/world.h"

// offset for uvs passed down to vertex
typedef struct
{
    VEC3(i8) u0, u1, v0, v1;
} FaceEdgeDirs;

static const Block* block_at_offset(const Chunk* chunk, ChunkPos chunk_pos, World* world,
                                    VEC3(i32) local, VEC3(i8) dir)
{
    VEC3(i32) npos = VEC3_ADD(local, dir);
    if (npos.x >= 0 && npos.x < CHUNK_SIZE && npos.y >= 0 && npos.y < CHUNK_SIZE
        && npos.z >= 0 && npos.z < CHUNK_SIZE)
        return chunk_get(chunk, VEC3_CAST(u8, npos));

    VEC3(i32) global = VEC3_ADD(VEC3_SCALE(chunk_pos, CHUNK_SIZE), npos);
    Chunk* nc = world_get_chunk(world, pos_glob_to_chunk(global));
    return nc ? chunk_get(nc, pos_glob_to_rel(global)) : NULL;
}

static bool same_type_at_offset(const Chunk* chunk, ChunkPos pos, World* world,
                                VEC3(i32) local, VEC3(i8) dir, BlockType type)
{
    const Block* nb = block_at_offset(chunk, pos, world, local, dir);
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
                                  VEC3(i32) local, int face, BlockType type, u8 flags)
{
    static const FaceEdgeDirs face_edge_dirs[6] = {
        {.u0 = {0, -1, 0}, .u1 = {0, 1, 0}, .v0 = {0, 0, 1}, .v1 = {0, 0, -1}}, // +X
        {.u0 = {0, 1, 0}, .u1 = {0, -1, 0}, .v0 = {0, 0, 1}, .v1 = {0, 0, -1}}, // -X
        {.u0 = {1, 0, 0}, .u1 = {-1, 0, 0}, .v0 = {0, 0, 1}, .v1 = {0, 0, -1}}, // +Y
        {.u0 = {-1, 0, 0}, .u1 = {1, 0, 0}, .v0 = {0, 0, 1}, .v1 = {0, 0, -1}}, // -Y
        {.u0 = {0, 1, 0}, .u1 = {0, -1, 0}, .v0 = {1, 0, 0}, .v1 = {-1, 0, 0}}, // +Z
        {.u0 = {0, -1, 0}, .u1 = {0, 1, 0}, .v0 = {1, 0, 0}, .v1 = {-1, 0, 0}}, // -Z
    };
    if (!(flags & (BLKTXT_CONNECTED4_BIT | BLKTXT_CONNECTED8_BIT)))
        return 0;

    const FaceEdgeDirs* d = &face_edge_dirs[face];
    bool u0 = same_type_at_offset(chunk, pos, world, local, d->u0, type);
    bool u1 = same_type_at_offset(chunk, pos, world, local, d->u1, type);
    bool v0 = same_type_at_offset(chunk, pos, world, local, d->v0, type);
    bool v1 = same_type_at_offset(chunk, pos, world, local, d->v1, type);

    if (!(flags & BLKTXT_CONNECTED8_BIT))
        return connected_mask_pack(u0, u1, v0, v1, false, false, false, false);

    bool nw = same_type_at_offset(chunk, pos, world, local, VEC3_ADD(d->u0, d->v0), type);
    bool ne = same_type_at_offset(chunk, pos, world, local, VEC3_ADD(d->u1, d->v0), type);
    bool sw = same_type_at_offset(chunk, pos, world, local, VEC3_ADD(d->u0, d->v1), type);
    bool se = same_type_at_offset(chunk, pos, world, local, VEC3_ADD(d->u1, d->v1), type);
    return connected_mask_pack(u0, u1, v0, v1, nw, ne, sw, se);
}

u32 generate_chunk_mesh(const Chunk* chunk, ChunkPos pos, World* world,
                        const FaceTexture block_faces[][6])
{
    u32 count = 0;

    static const VEC3(i8) dirs[6] = {
        {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1},
    };

    for (u8 x = 0; x < CHUNK_SIZE; x++)
        for (u8 y = 0; y < CHUNK_SIZE; y++)
            for (u8 z = 0; z < CHUNK_SIZE; z++)
            {
                const Block* b = chunk_get(chunk, (VEC3(u8)){x, y, z});
                if (b->type == BLK_AIR)
                    continue;

                for (int f = 0; f < 6; f++)
                {
                    VEC3(u8)
                    npos = {(u8)(x + dirs[f].x), (u8)(y + dirs[f].y),
                            (u8)(z + dirs[f].z)};

                    if (chunk_is_in_bound(npos)
                        && chunk_get(chunk, npos)->type != BLK_AIR)
                        continue;

                    const FaceTexture* ft = &block_faces[b->type][f];
                    VEC3(i32) local = {x, y, z};
                    u8 neighbor_mask = connected_neighbor_mask(chunk, pos, world, local,
                                                               f, b->type, ft->flags);

                    chunk->bucket_ptr[count++] = (Face){
                        .pos = VEC3_ADD(VEC3_SCALE(pos, CHUNK_SIZE), local),
                        .face_id = f,
                        .texture_id = face_texture_to_index(ft, neighbor_mask, false, 0),
                    };
                }
            }

    return count;
}

