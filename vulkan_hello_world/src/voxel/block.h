#pragma once

#include "utils/utils.h"
#include "voxel/block/app_controller.h"

#define XBLOCK(X)                                                                        \
    X(BLK_AIR, .has_mesh = false)                                                        \
    X(BLK_GRASS)                                                                         \
    X(BLK_DIRT)                                                                          \
    X(BLK_SAND)                                                                          \
    X(BLK_RED_SAND)                                                                      \
    X(BLK_STONE)                                                                         \
    X(BLK_DARKSTONE)                                                                     \
    X(BLK_OAK)                                                                           \
    X(BLK_LAMP, .on_place = lamp_on_place, .on_break = lamp_on_break)                    \
    X(BLK_APP_CONTROLLER, .on_break = app_controller_on_break,                           \
      .on_place = app_controller_on_place, .on_interact = app_controller_on_interact)    \
    //    X(BLK_SCREEN, .texture_connect_with = {BLK_APP_CONTROLLER})

typedef enum BlockType : u16
{
#define XBLOCK_FN(NAME, ...) NAME,
    XBLOCK(XBLOCK_FN)
#undef XBLOCK_FN
    BLOCK_COUNT
} BlockType;

typedef struct Block
{
    BlockType type;
    u16 state;
    u16 entity_id;
} Block;

struct Chunk;

typedef struct BlockBehaviour
{
    bool has_mesh;
    BlockType texture_connect_with[10];
    void (*on_place)(Block*, Chunk*);
    void (*on_break)(Block*, Chunk*);
    void (*on_interact)(Block*, Chunk*);
    void (*on_update)(Block*, Chunk*);
    char* str_repr;
} BlockBehaviour;

extern const BlockBehaviour blockBehaviours[BLOCK_COUNT];

extern const VEC3(i8) BLOCK_NEIGHB_DIRS[6];
