#include "face_texture_build.h"

#include <stdlib.h>

#include "utils/type.h"
#include "utils/utils.h"
#include "voxel/textures/array_atlas.h"
#include "voxel/textures/base_texture_enum.h"
#include "voxel/textures/connected_texture_build.h"

static void consume_leaf(NodeId id, const TextureNode* nodes, FaceTexture* res,
                         ArrayAtlas* atlas)
{
    const TextureNode* n = &nodes[id];
    if (res->frame_count == 0)
        res->frame_count = n->leaf.count;
    ASSERT(res->frame_count == n->leaf.count,
           "mismatched animation frame counts within one face-texture bundle (node %u)",
           id);

    atlas_push_from_base_texture(atlas, n->leaf.texture_id, n->leaf.count);
}

static void consume_connected(NodeId id, const TextureNode* nodes, FaceTexture* res,
                              ArrayAtlas* atlas)
{
    const TextureNode* n = &nodes[id];
    if (!(n->type & (BLKTXT_CONNECTED4_BIT | BLKTXT_CONNECTED8_BIT)))
    {
        consume_leaf(id, nodes, res, atlas);
        return;
    }

    res->flags |= n->type;
    if (res->frame_count == 0)
        res->frame_count = 1;
    ASSERT(res->frame_count == 1,
           "connected face-textures do not support animation (node %u)", id);

    u8* inner_px = load_rgba(TexturePaths[nodes[n->connected.inner].leaf.texture_id]);
    u8* border_px = load_rgba(TexturePaths[nodes[n->connected.border].leaf.texture_id]);

    if (n->type & BLKTXT_CONNECTED8_BIT)
        bake_connected8(atlas, inner_px, border_px);
    else
        bake_connected4(atlas, inner_px, border_px);

    free(inner_px);
    free(border_px);
}

static void consume_activated(NodeId id, const TextureNode* nodes, FaceTexture* res,
                              ArrayAtlas* atlas)
{
    const TextureNode* n = &nodes[id];
    if (!(n->type & BLKTXT_ACTIVATED_BIT))
    {
        consume_connected(id, nodes, res, atlas);
        return;
    }
    res->flags |= BLKTXT_ACTIVATED_BIT;
    consume_connected(n->activated.off, nodes, res, atlas);
    consume_connected(n->activated.on, nodes, res, atlas);
}

static void consume_variant(NodeId id, const TextureNode* nodes, FaceTexture* res,
                            ArrayAtlas* atlas)
{
    const TextureNode* n = &nodes[id];
    if (!(n->type & BLKTXT_VARIANT_BIT))
    {
        consume_activated(id, nodes, res, atlas);
        return;
    }
    res->flags |= BLKTXT_VARIANT_BIT;
    res->variant_count = n->variant.count;
    for (u8 i = 0; i < n->variant.count; i++)
        consume_activated(n->variant.children[i], nodes, res, atlas);
}

FaceTexture face_texture_build(const TextureNode* nodes, NodeId root, ArrayAtlas* atlas)
{
    FaceTexture res = {
        .base_id = atlas_get_current_slot(atlas),
        .flags = 0,
        .variant_count = 1,
        .frame_count = 0,
    };

    consume_variant(root, nodes, &res, atlas);

    return res;
}
