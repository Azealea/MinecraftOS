#pragma once

#include <stdint.h>

#define BLOCK_FACE_COUNT 6

typedef enum BlockType : uint16_t
{
    BLK_AIR = 0,
    BLK_GRASS,
    BLK_DIRT,
    BLK_SAND,
    BLK_RED_SAND,
    BLK_STONE,
    BLK_DARKSTONE,
    BLK_OAK,
    BLK_LAMP,
    // BLK_APP_CONTROLLER,
    // BLK_SCREEN,
    BLOCK_COUNT
} BlockType;

typedef struct Block
{
    BlockType type;
    uint16_t state;
    uint16_t entity_id;
} Block;
