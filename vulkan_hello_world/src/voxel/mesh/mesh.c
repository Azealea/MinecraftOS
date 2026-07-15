#include "mesh.h"

#include "utils/utils.h"
#include "voxel/textures/face_texture.h"

uint32_t generate_chunk_mesh(const Chunk* chunk, ChunkPos pos, Face* dst,
                             const FaceTexture (*block_faces)[BLOCK_FACE_COUNT])
{
    uint32_t count = 0;

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

                    dst[count++] = (Face){
                        .pos =
                            {
                                .x = (i32)(pos.x * CHUNK_SIZE + x),
                                .y = (i32)(pos.y * CHUNK_SIZE + y),
                                .z = (i32)(pos.z * CHUNK_SIZE + z),
                            },
                        .face_id = f,
                        .texture_id =
                            face_texture_resolve(&block_faces[b->type][f], 0, 0, 0),
                    };
                }
            }

    return count;
}

void world_remesh_chunk(World* w, ChunkPos pos,
                        const FaceTexture (*block_faces)[BLOCK_FACE_COUNT])
{
    Chunk* c = world_get_chunk(w, pos);
    ASSERT(c != NULL, "world_remesh_chunk: chunk " VEC3_FMT " does not exist",
           VEC3_ARGS(pos));
    if (c->bucket_index == NO_BUCKET)
        c->bucket_index = bucket_alloc_acquire(&w->mesh);
    c->face_count =
        generate_chunk_mesh(c, pos, bucket_ptr(&w->mesh, c->bucket_index), block_faces);
}
