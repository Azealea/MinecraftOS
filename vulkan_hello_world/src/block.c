#include "block.h"

#include "texture_enum.h"

#define MAP_BARREL(SIDE, TOPBOT)                                               \
    {(SIDE), (SIDE), (TOPBOT), (TOPBOT), (SIDE), (SIDE)}

#define MAP_DONUT(SIDE, TOP, BOT) {(SIDE), (SIDE), (SIDE), (SIDE), (TOP), (BOT)}

#define MAP_UNIFO(TEXT) {(TEXT), (TEXT), (TEXT), (TEXT), (TEXT), (TEXT)}

#define MAP_CROWN(SIDE, FRONT, BOT, TOP)                                       \
    {(SIDE), (FRONT), (TOP), (BOT), (SIDE), (SIDE)}

const int BlockTexture[][FACE_COUNT] = {
    [BLK_DIRT] = MAP_UNIFO(TEX_DIRT),
    [BLK_GRASS] = MAP_DONUT(TEX_GRASS_SIDE, TEX_GRASS, TEX_DIRT),
    [BLK_STONE] = MAP_UNIFO(TEX_STONE),
};
