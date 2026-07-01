#pragma once

#include "voxel/block.h"
#include "voxel/textures/array_atlas.h"

void load_texture_into_atlas(ArrayAtlas* atlas, FaceTexture (*block_faces)[FACE_COUNT]);
void debug_print_block_faces(const FaceTexture (*block_faces)[FACE_COUNT]);
