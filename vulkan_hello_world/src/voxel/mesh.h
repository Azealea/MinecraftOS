#pragma once

#include "voxel/chunk.h"
#include "voxel/face.h"
#include "voxel/textures/face_texture.h"

uint32_t generate_chunk_mesh(const Chunk* chunk, ChunkPos pos, Face* dst,
                             const FaceTexture (*block_faces)[FACE_COUNT]);
