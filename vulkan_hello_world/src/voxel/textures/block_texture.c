#include "block_texture.h"

#include <stdio.h>

#include "base_texture_enum.h"
#include "utils/utils.h"

#define LEAF(TID) {.type = 0, .leaf = {.texture_id = (TID), .count = 1}}
#define LEAF_ANIM(TID, N)                                                                \
    {                                                                                    \
        .type = 0, .leaf = {.texture_id = (TID), .count = (N) }                          \
    }
#define CONNECTED4(INNER, EDGE, CORN)                                                    \
    {                                                                                    \
        .type = BLKTXT_CONNECTED4_BIT,                                                   \
        .connected = {.inner = (INNER), .edge = (EDGE), .corner = (CORN)},               \
    }
#define CONNECTED8(INNER, EDGE, CORN)                                                    \
    {                                                                                    \
        .type = BLKTXT_CONNECTED8_BIT,                                                   \
        .connected = {.inner = (INNER), .edge = (EDGE), .corner = (CORN)},               \
    }
#define ACTIVATED(OFF, ON)                                                               \
    {                                                                                    \
        .type = BLKTXT_ACTIVATED_BIT, .activated = {.off = (OFF), .on = (ON) }           \
    }
#define VARIANT(...)                                                                     \
    {                                                                                    \
        .type = BLKTXT_VARIANT_BIT,                                                      \
        .variant = {.children = {__VA_ARGS__},                                           \
                    .count = COUNTOF(((NodeId[]){__VA_ARGS__}))},                        \
    }

#define BARREL(SIDE, TOPBOT) {(SIDE), (SIDE), (SIDE), (SIDE), (TOPBOT), (TOPBOT)}
#define DONUT(SIDE, TOP, BOT) {(SIDE), (SIDE), (SIDE), (SIDE), (TOP), (BOT)}
#define UNIFO(TEXT) {(TEXT), (TEXT), (TEXT), (TEXT), (TEXT), (TEXT)}

#define NODE_LIST(X)                                                                     \
    X(NODE_DIRT, LEAF(TEX_DIRT))                                                         \
    X(NODE_GRASS_SIDE, LEAF(TEX_GRASS_SIDE))                                             \
    X(NODE_GRASS_TOP, LEAF(TEX_GRASS))                                                   \
    X(NODE_STONE, LEAF(TEX_STONE))                                                       \
    X(NODE_OAK, LEAF(TEX_OAK))                                                           \
    X(NODE_OAK_INNER, LEAF(TEX_OAK_INNER))                                               \
    X(NODE_LAMP_ON, LEAF(TEX_LAMP_ON))                                                   \
    X(NODE_LAMP_OFF, LEAF(TEX_LAMP_OFF))                                                 \
    X(NODE_LAMP_ACTIVATED, ACTIVATED(NODE_LAMP_ON, NODE_LAMP_OFF))

enum
{
    NODE_NONE = 0, // unset value
#define X(NAME, VALUE) NAME,
    NODE_LIST(X)
#undef X
        NODE_COUNT,
};

const TextureNode BlockTextureNodes[NODE_COUNT] = {
#define X(NAME, VALUE) [NAME] = VALUE,
    NODE_LIST(X)
#undef X
};

#undef NODE_LIST

const NodeId BlockTextures[BLOCK_COUNT][BLOCK_FACE_COUNT] = {
    [BLK_AIR] = UNIFO(NODE_DIRT), // never actually used
    [BLK_DIRT] = UNIFO(NODE_DIRT),
    [BLK_GRASS] = DONUT(NODE_GRASS_SIDE, NODE_GRASS_TOP, NODE_DIRT),
    [BLK_STONE] = UNIFO(NODE_STONE),
    [BLK_OAK] = BARREL(NODE_OAK, NODE_OAK_INNER),
    [BLK_LAMP] = UNIFO(NODE_LAMP_ACTIVATED),
};

#undef LEAF
#undef LEAF_ANIM
#undef CONNECTED4
#undef CONNECTED8
#undef ACTIVATED
#undef VARIANT
#undef BARREL
#undef DONUT
#undef UNIFO

void validate_block_textures(void)
{
    for (int b = 0; b < BLOCK_COUNT; b++)
        for (int f = 0; f < BLOCK_FACE_COUNT; f++)
            ASSERT(BlockTextures[b][f] != NODE_NONE,
                   "block %d has no face-texture description for face %d "
                   "(forgot to add it to BlockTextures?)",
                   b, f);
}

static void build_texture_look_up(const TextureNode* nodes,
                                  const NodeId roots[][BLOCK_FACE_COUNT],
                                  FaceTexture destination[][BLOCK_FACE_COUNT],
                                  ArrayAtlas* atlas)
{
    for (int b = 0; b < BLOCK_COUNT; b++)
        for (int f = 0; f < BLOCK_FACE_COUNT; f++)
            destination[b][f] = face_texture_build(nodes, roots[b][f], atlas);
}

void load_texture_into_atlas(ArrayAtlas* atlas,
                             FaceTexture (*block_faces)[BLOCK_FACE_COUNT])
{
    validate_block_textures();
    build_texture_look_up(BlockTextureNodes, BlockTextures, block_faces, atlas);
}

void debug_print_block_textures(const FaceTexture (*block_faces)[BLOCK_FACE_COUNT])
{
    for (int blk = 0; blk < BLOCK_COUNT; blk++)
    {
        printf("Block %d:\n", blk);
        for (int face = 0; face < BLOCK_FACE_COUNT; face++)
        {
            const FaceTexture* f = &block_faces[blk][face];
            printf("  face %d: base_id=%-4u flags=0x%02x variant_count=%-3u "
                   "frame_count=%-3u | "
                   "connected=%s activated=%s variant=%s\n",
                   face, f->base_id, f->flags, f->variant_count, f->frame_count,
                   (f->flags & BLKTXT_CONNECTED8_BIT)       ? "8"
                       : (f->flags & BLKTXT_CONNECTED4_BIT) ? "4"
                                                            : "-",
                   (f->flags & BLKTXT_ACTIVATED_BIT) ? "yes" : "no",
                   (f->flags & BLKTXT_VARIANT_BIT) ? "yes" : "no");
        }
    }
}
