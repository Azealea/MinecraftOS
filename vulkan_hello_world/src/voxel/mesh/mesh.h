#pragma once

#include "voxel/textures/resolve/face_texture.h"
#include "voxel/world/world.h"

u32 generate_chunk_mesh(const Chunk* chunk, ChunkPos pos, World* world,
                        const FaceTexture block_faces[][6]);
