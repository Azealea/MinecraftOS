#pragma once

#include "utils/utils.h"

typedef enum BlockType : u16
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
    BLK_APP_CONTROLLER,
    // BLK_SCREEN,
    BLOCK_COUNT
} BlockType;

typedef struct Block
{
    BlockType type;
    u16 state;
    u16 entity_id;
} Block;
