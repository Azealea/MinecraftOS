#pragma once

#include <stdint.h>

#include "voxel/textures/face_texture.h"

typedef enum BlockType : uint16_t
{
    BLK_AIR = 0,
    BLK_GRASS,
    BLK_DIRT,
    BLK_STONE,
    BLK_OAK,
    BLK_LAMP,
    // BLK_APP_CONTROLLER,
    // BLK_SCREEN,
    BLOCK_COUNT
} BlockType;

#define FACE_COUNT 6

typedef struct Block
{
    BlockType type;
    uint16_t state;
    uint16_t entity_id;
} Block;

extern const FaceTextureBuilder BlockFaceBuilders[][FACE_COUNT];
extern const int BlockFaceBuildersCount;
