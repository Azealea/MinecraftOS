#pragma once

#include "voxel/block.h"
#include "voxel/world.h"

void generate_chunk_mesh(World* world, ChunkPos pos,
                         const FaceTexture (*block_faces)[FACE_COUNT]);
