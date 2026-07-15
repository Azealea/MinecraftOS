#pragma once

#include "face.h"
#include "voxel/textures/face_texture.h"
#include "voxel/world/world.h"

uint32_t generate_chunk_mesh(const Chunk* chunk, ChunkPos pos, Face* dst,
                             const FaceTexture (*block_faces)[BLOCK_FACE_COUNT]);

void world_remesh_chunk(World* w, ChunkPos pos,
                        const FaceTexture (*block_faces)[BLOCK_FACE_COUNT]);
