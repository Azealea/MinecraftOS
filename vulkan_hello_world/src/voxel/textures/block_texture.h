#pragma once

#include "voxel/block.h"
#include "voxel/textures/face_texture.h"

extern const TextureNode BlockTextureNodes[];
extern const NodeId BlockTextures[BLOCK_COUNT][BLOCK_FACE_COUNT];

void validate_block_textures(void);

void load_texture_into_atlas(ArrayAtlas* atlas,
                             FaceTexture (*block_faces)[BLOCK_FACE_COUNT]);
void debug_print_block_textures(const FaceTexture (*block_faces)[BLOCK_FACE_COUNT]);
