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
    X(BLK_OAK, .rotates = true)                                                          \
    X(BLK_LAMP, .on_place = network_on_place, .on_break = network_on_break)              \
    X(BLK_APP_BRIDGE, .on_place = network_on_place, .on_break = network_on_break)        \
    X(BLK_APP_SCREEN, .on_place = network_on_place, .on_break = network_on_break,        \
      .rotates = true)                                                                   \
    X(BLK_APP_CONTROLLER, .on_break = app_controller_on_break,                           \
      .on_place = app_controller_on_place, .on_interact = app_controller_on_interact)    \
    X(BLK_SNAKE_TEST, .rotates = true)

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
    bool rotates;
    BlockType texture_connect_with[10];
    void (*on_place)(Block*, Chunk*);
    void (*on_break)(Block*, Chunk*);
    void (*on_interact)(Block*, Chunk*);
    void (*on_update)(Block*, Chunk*);
    char* str_repr;
} BlockBehaviour;

extern const BlockBehaviour blockBehaviours[BLOCK_COUNT];
