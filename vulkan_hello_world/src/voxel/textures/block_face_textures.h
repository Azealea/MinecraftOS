#pragma once

#include "voxel/block.h"
#include "voxel/textures/face_texture.h"

extern const FaceTextureNode FaceTextureNodes[];
extern const NodeId BlockFaceRoots[BLOCK_COUNT][BLOCK_FACE_COUNT];

void validate_block_face_roots(void);

void load_texture_into_atlas(ArrayAtlas* atlas,
                             FaceTexture (*block_faces)[BLOCK_FACE_COUNT]);
void debug_print_block_faces(const FaceTexture (*block_faces)[BLOCK_FACE_COUNT]);
