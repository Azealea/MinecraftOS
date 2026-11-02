#pragma once

#include "voxel/direction.h"
#include "voxel/textures/builder/array_atlas.h"
#include "voxel/textures/resolve/face_texture.h"
#include "voxel/textures/texture_flags.h"

typedef u16 txtNodeId;
#define TEXTURE_NODE_VARIANT_MAX_CHILDREN 8

struct textureNode
{
    enum textureFlags type; // 0 = leaf
    union
    {
        struct
        {
            u32 texture_id;
            u8 count;
        } leaf;
        struct
        {
            txtNodeId inner, border;
        } connected;
        struct
        {
            txtNodeId off, on;
        } activated;
        struct
        {
            txtNodeId children[TEXTURE_NODE_VARIANT_MAX_CHILDREN];
            u8 count;
        } variant;
    };
};
typedef struct textureNode textureNode;

FaceTexture face_texture_build(const textureNode* nodes, txtNodeId root,
                               VECTOR(texture_t)* atlas);

VECTOR(texture_t) build_texture_look_up(FaceTexture destination[][DIRECTION_COUNT]);
bool validate_block_textures(void);
