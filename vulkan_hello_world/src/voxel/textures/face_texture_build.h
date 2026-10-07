#pragma once

#include "voxel/textures/array_atlas.h"
#include "voxel/textures/face_texture.h"
#include "voxel/textures/texture_node.h"

FaceTexture face_texture_build(const TextureNode* nodes, NodeId root, ArrayAtlas* atlas);
