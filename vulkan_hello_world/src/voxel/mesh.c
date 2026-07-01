#include "voxel/mesh.h"

#include "voxel/textures/face_texture.h"

void generate_chunk_mesh(World* world, ChunkPos pos,
                         const FaceTexture (*block_faces)[FACE_COUNT])
{
    const Chunk* chunk = world_get_or_add_chunk(world, pos);
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

                    world->faces[count++] = (Face){
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

    world->face_count = count;
}
