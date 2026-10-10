#pragma once

#include "voxel/mesh/bucket_alloc.h"
#include "voxel/textures/resolve/face_texture.h"
#include "voxel/world/world.h"

typedef struct App App;

typedef struct Mesher
{
    VECTOR(ChunkPos) dirty; // chunks needing a mesh rebuild
    BucketAlloc mesh;
    FaceTexture Block2TextureMap[BLOCK_COUNT][6];
} Mesher;

Mesher mesher_init(void);
void mesher_free(Mesher* m);

void mesher_mesh_chunk(Mesher* m, World* w, ChunkPos pos);
void rebuild_dirty_meshes(App* app);
