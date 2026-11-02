#include "texture_tree.h"

#include "connected_texture_build.h"
#include "voxel/block.h"

// VVVV description of the "dynamic" textures

#define LEAF(TID) {.type = 0, .leaf = {.texture_id = (TID), .count = 1}}
#define LEAF_ANIM(TID, N)                                                                \
    {                                                                                    \
        .type = 0, .leaf = {.texture_id = (TID), .count = (N)}                           \
    }
#define CONNECTED4(INNER, BORDER)                                                        \
    {                                                                                    \
        .type = BLKTXT_CONNECTED4_BIT,                                                   \
        .connected = {.inner = (INNER), .border = (BORDER)},                             \
    }
#define CONNECTED8(INNER, BORDER)                                                        \
    {                                                                                    \
        .type = BLKTXT_CONNECTED8_BIT,                                                   \
        .connected = {.inner = (INNER), .border = (BORDER)},                             \
    }
#define ACTIVATED(OFF, ON)                                                               \
    {                                                                                    \
        .type = BLKTXT_ACTIVATED_BIT, .activated = {.off = (OFF), .on = (ON)}            \
    }
#define VARIANT(...)                                                                     \
    {                                                                                    \
        .type = BLKTXT_VARIANT_BIT,                                                      \
        .variant = {.children = {__VA_ARGS__},                                           \
                    .count = COUNTOF(((txtNodeId[]){__VA_ARGS__}))},                     \
    }

#define NODE_LIST(X)                                                                     \
    X(NODE_DIRT, LEAF(TEX_DIRT))                                                         \
    X(NODE_GRASS_SIDE, LEAF(TEX_GRASS_SIDE))                                             \
    X(NODE_GRASS_TOP1, LEAF(TEX_GRASS1))                                                 \
    X(NODE_GRASS_TOP2, LEAF(TEX_GRASS2))                                                 \
    X(NODE_GRASS_TOP, VARIANT(NODE_GRASS_TOP1, NODE_GRASS_TOP2))                         \
    X(NODE_STONE, LEAF(TEX_STONE))                                                       \
    X(NODE_DARKSTONE, LEAF(TEX_DARKSTONE))                                               \
    X(NODE_SAND, LEAF(TEX_SAND))                                                         \
    X(NODE_RED_SAND, LEAF(TEX_RED_SAND))                                                 \
    X(NODE_OAK, LEAF(TEX_OAK))                                                           \
    X(NODE_OAK_INNER, LEAF(TEX_OAK_INNER))                                               \
    X(NODE_LAMP_ON, LEAF(TEX_LAMP_ON))                                                   \
    X(NODE_LAMP_OFF, LEAF(TEX_LAMP_OFF))                                                 \
    X(NODE_LAMP_ACTIVATED, ACTIVATED(NODE_LAMP_OFF, NODE_LAMP_ON))                       \
    X(NODE_APP_BRIDGE_INNER, LEAF(TEX_APP_BRIDGE))                                       \
    X(NODE_APP_SCREEN, LEAF(TEX_APP_SCREEN))                                             \
    X(NODE_BORDER_ACTIVE, LEAF(TEX_BORDER_ACTIVE))                                       \
    X(NODE_APP_BRIDGE, CONNECTED8(NODE_APP_BRIDGE_INNER, NODE_BORDER_ACTIVE))            \
    X(NODE_APP_CONTROLLER, LEAF(TEX_APP_CONTROLLER))                                     \
    X(NODE_TEST_FRONT, LEAF(TEX_TEST_FRONT))                                             \
    X(NODE_TEST_BACK, LEAF(TEX_TEST_BACK))                                               \
    X(NODE_TEST_LEFT, LEAF(TEX_TEST_LEFT))                                               \
    X(NODE_TEST_RIGHT, LEAF(TEX_TEST_RIGHT))                                             \
    X(NODE_TEST_TOP, LEAF(TEX_TEST_UP))                                                  \
    X(NODE_TEST_BOTTOM, LEAF(TEX_TEST_DOWN))

enum
{
    NODE_NONE = 0, // unset value
#define X(NAME, VALUE) NAME,
    NODE_LIST(X)
#undef X
    NODE_COUNT,
};

const struct textureNode BlockTextureNodes[NODE_COUNT] = {
#define X(NAME, VALUE) [NAME] = VALUE,
    NODE_LIST(X)
#undef X
};

#undef NODE_LIST

#undef LEAF
#undef LEAF_ANIM
#undef CONNECTED4
#undef CONNECTED8
#undef ACTIVATED
#undef VARIANT

// VVVV mapping of the texture on the blocks

#define BARREL(SIDE, TOPBOT) {(SIDE), (SIDE), (SIDE), (SIDE), (TOPBOT), (TOPBOT)}
#define DONUT(SIDE, TOP, BOT) {(SIDE), (SIDE), (SIDE), (SIDE), (TOP), (BOT)}
#define UNIFO(TEXT) {(TEXT), (TEXT), (TEXT), (TEXT), (TEXT), (TEXT)}
#define FRONT(SIDE, FACE) {(FACE), (SIDE), (SIDE), (SIDE), (SIDE), (SIDE)}
#define SNAKE(FRONT, BACK, SIDES, TOP, BOTTOM)                                           \
    {(FRONT), (BACK), (TOP), (BOTTOM), (SIDES), (SIDES)}
#define ALLSIDES(FRONT, BACK, TOP, BOTTOM, RIGHT, LEFT)                                  \
    {(FRONT), (BACK), (TOP), (BOTTOM), (RIGHT), (LEFT)}

const txtNodeId BlockTextures[BLOCK_COUNT][DIRECTION_COUNT] = {
    [BLK_AIR] = UNIFO(NODE_NONE),
    [BLK_DIRT] = UNIFO(NODE_DIRT),
    [BLK_GRASS] = DONUT(NODE_GRASS_SIDE, NODE_GRASS_TOP, NODE_DIRT),
    [BLK_STONE] = UNIFO(NODE_STONE),
    [BLK_DARKSTONE] = UNIFO(NODE_DARKSTONE),
    [BLK_SAND] = UNIFO(NODE_SAND),
    [BLK_RED_SAND] = UNIFO(NODE_RED_SAND),
    [BLK_OAK] = SNAKE(NODE_OAK_INNER, NODE_OAK_INNER, NODE_OAK, NODE_OAK, NODE_OAK),
    [BLK_LAMP] = UNIFO(NODE_LAMP_ACTIVATED),
    [BLK_APP_BRIDGE] = UNIFO(NODE_APP_BRIDGE),
    [BLK_APP_SCREEN] = FRONT(NODE_APP_BRIDGE, NODE_APP_SCREEN),
    [BLK_APP_CONTROLLER] = UNIFO(NODE_APP_CONTROLLER),
    [BLK_SNAKE_TEST] = ALLSIDES(NODE_TEST_FRONT, NODE_TEST_BACK, NODE_TEST_TOP,
                                NODE_TEST_BOTTOM, NODE_TEST_RIGHT, NODE_TEST_LEFT),
};

#undef BARREL
#undef DONUT
#undef UNIFO
#undef FRONT
#undef SNAKE
#undef ALLSIDES

// VVV building method

static void consume_leaf(const struct textureNode* nodes, txtNodeId id, FaceTexture* res,
                         VECTOR(texture_t)* atlas)
{
    const struct textureNode* n = &nodes[id];
    if (res->frame_count == 0)
        res->frame_count = n->leaf.count;
    ASSERT(res->frame_count == n->leaf.count,
           "mismatched animation frame counts within one face-texture bundle (node %u)",
           id);

    atlas_push_from_base_texture(atlas, n->leaf.texture_id, n->leaf.count);
}

static void consume_connected(const textureNode* nodes, txtNodeId id, FaceTexture* res,
                              VECTOR(texture_t)* atlas)
{
    const textureNode* n = &nodes[id];
    if (!(n->type & (BLKTXT_CONNECTED4_BIT | BLKTXT_CONNECTED8_BIT)))
    {
        consume_leaf(nodes, id, res, atlas);
        return;
    }

    res->flags |= n->type;
    if (res->frame_count == 0)
        res->frame_count = 1;
    ASSERT(res->frame_count == 1,
           "connected face-textures do not support animation (node %u)", id);

    texture_t inner_px =
        load_rgba(TexturePaths[nodes[n->connected.inner].leaf.texture_id]);
    texture_t border_px =
        load_rgba(TexturePaths[nodes[n->connected.border].leaf.texture_id]);

    if (n->type & BLKTXT_CONNECTED8_BIT)
        bake_connected8(atlas, inner_px, border_px);
    else
        bake_connected4(atlas, inner_px, border_px);

    free(inner_px);
    free(border_px);
}

static void consume_activated(const textureNode* nodes, txtNodeId id, FaceTexture* res,
                              VECTOR(texture_t)* atlas)
{
    const textureNode* n = &nodes[id];
    if (!(n->type & BLKTXT_ACTIVATED_BIT))
    {
        consume_connected(nodes, id, res, atlas);
        return;
    }
    res->flags |= BLKTXT_ACTIVATED_BIT;
    consume_connected(nodes, n->activated.off, res, atlas);
    consume_connected(nodes, n->activated.on, res, atlas);
}

static void consume_variant(const textureNode* nodes, txtNodeId id, FaceTexture* res,
                            VECTOR(texture_t)* atlas)
{
    const textureNode* n = &nodes[id];
    if (!(n->type & BLKTXT_VARIANT_BIT))
    {
        consume_activated(nodes, id, res, atlas);
        return;
    }
    res->flags |= BLKTXT_VARIANT_BIT;
    res->variant_count = n->variant.count;
    for (u8 i = 0; i < n->variant.count; i++)
        consume_activated(nodes, n->variant.children[i], res, atlas);
}

FaceTexture face_texture_build(const textureNode* nodes, txtNodeId root,
                               VECTOR(texture_t)* atlas)
{
    FaceTexture res = {
        .base_id = atlas->size,
        .flags = 0,
        .variant_count = 1,
        .frame_count = 0,
    };

    consume_variant(nodes, root, &res, atlas);

    return res;
}

VECTOR(texture_t) build_texture_look_up(FaceTexture destination[][DIRECTION_COUNT])
{
    VECTOR(texture_t) atlas = VECTOR_INIT_CAP(texture_t, MAX_TEXTURE);
    FaceTexture cache[NODE_COUNT];
    bool built[NODE_COUNT] = {0};

    for (int b = 0; b < BLOCK_COUNT; b++)
        for (Direction f = 0; f < DIRECTION_COUNT; f++)
        {
            txtNodeId root = BlockTextures[b][f];
            if (root == NODE_NONE) // block without texture (eg: air)
                continue;
            if (!built[root])
            {
                cache[root] = face_texture_build(BlockTextureNodes, root, &atlas);
                built[root] = true;
            }
            destination[b][f] = cache[root];
        }

    return atlas;
}

bool validate_block_textures(void)
{
    bool no_error = true;
    for (int b = 0; b < BLOCK_COUNT; b++)
    {
        if (b == BLK_AIR)
            continue;
        for (Direction f = 0; f < DIRECTION_COUNT; f++)
        {
            if (BlockTextures[b][f] == NODE_NONE)
            {
                ERROR("block %d has no face-texture description for face %d (forgot to "
                      "add it to BlockTextures?)",
                      b, f);
                no_error = false;
            }
        }
    }
    return no_error;
}
