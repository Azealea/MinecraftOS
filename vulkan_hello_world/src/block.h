#pragma once

typedef enum BlockType
{
    BLK_AIR = 0,
    BLK_GRASS,
    BLK_DIRT,
    BLK_STONE,
} BlockType;

#define FACE_COUNT 6

extern const int BlockTexture[][FACE_COUNT];

// enum BlockSateAxe
//{
//     BLK_STATE_AXE_Y,
//     BLK_STATE_AXE_X,
//     BLK_STATE_AXE_Z,
// };
//
// enum BlockSateFacing
//{
//     BLK_STATE_FACING_XP,
//     BLK_STATE_FACING_XM,
//     BLK_STATE_FACING_ZP,
//     BLK_STATE_FACING_ZM,
// };
//
// enum BlockSateRotaion
//{
//     BLK_STATE_ROTATION_XP,
//     BLK_STATE_ROTATION_XM,
//     BLK_STATE_ROTATION_ZP,
//     BLK_STATE_ROTATION_ZM,
//     BLK_STATE_ROTATION_YP,
//     BLK_STATE_ROTATION_YM,
// };

typedef struct Block
{
    BlockType type;
    // union
    //{
    // enum BlockSateAxe axis;
    // enum BlockSateFacing facing;
    // };
} Block;
