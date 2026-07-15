#include "face_texture.h"

#include "utils/utils.h"
#include "voxel/textures/array_atlas.h"

uint16_t face_texture_resolve(const FaceTexture* f, uint8_t neighbor_mask, bool activated,
                              uint8_t hash_pos)
{
    const uint8_t connected_size = (f->flags & FACETXT_CONNECTED8_BIT) ? 46
        : (f->flags & FACETXT_CONNECTED4_BIT)                          ? 16
                                                                       : 1;
    const uint8_t activated_size = (f->flags & FACETXT_ACTIVATED_BIT) ? 2 : 1;

    const uint8_t variant_pos =
        (f->flags & FACETXT_VARIANT_BIT) ? hash_pos % f->variant_count : 0;
    const uint8_t activated_pos =
        (f->flags & FACETXT_ACTIVATED_BIT) ? (activated ? 1 : 0) : 0;
    const uint8_t connected_pos = 0;
    // (connected_size > 1) ? connected_mask_to_offset[neighbor_mask] :
    // 0;

    int res = f->base_id
        + ((uint32_t)variant_pos * activated_size * connected_size
           + (uint32_t)activated_pos * connected_size + (uint32_t)connected_pos)
            * f->frame_count;
    //    printf("resolved :%d\n", res);
    return res;
}

static void consume_leaf(NodeId id, const FaceTextureNode* nodes, FaceTexture* res,
                         ArrayAtlas* atlas)
{
    const FaceTextureNode* n = &nodes[id];
    if (res->frame_count == 0)
        res->frame_count = n->leaf.count;
    ASSERT(res->frame_count == n->leaf.count,
           "mismatched animation frame counts within one face-texture composite "
           "(node %u)",
           id);

    atlas_push_from_base_texture(atlas, n->leaf.texture_id, n->leaf.count);
}

static void consume_connected(NodeId id, const FaceTextureNode* nodes, FaceTexture* res,
                              ArrayAtlas* atlas)
{
    const FaceTextureNode* n = &nodes[id];
    if (!(n->type & (FACETXT_CONNECTED4_BIT | FACETXT_CONNECTED8_BIT)))
    {
        consume_leaf(id, nodes, res, atlas);
        return;
    }

    res->flags |= n->type;

    uint8_t cases = (n->type & FACETXT_CONNECTED8_BIT) ? 46 : 16;

    for (uint8_t c = 0; c < cases; c++)
    {
        NodeId src;
        // TODO: connected_offset_to_part[c] dispatch
        switch (0)
        {
        case 0:
            src = n->connected.inner;
            break;
        case 1:
            src = n->connected.edge;
            break;
        default:
            src = n->connected.corner;
            break;
        }
        consume_leaf(src, nodes, res, atlas);
    }
}

static void consume_activated(NodeId id, const FaceTextureNode* nodes, FaceTexture* res,
                              ArrayAtlas* atlas)
{
    const FaceTextureNode* n = &nodes[id];
    if (!(n->type & FACETXT_ACTIVATED_BIT))
    {
        consume_connected(id, nodes, res, atlas);
        return;
    }
    res->flags |= FACETXT_ACTIVATED_BIT;
    consume_connected(n->activated.off, nodes, res, atlas);
    consume_connected(n->activated.on, nodes, res, atlas);
}

static void consume_variant(NodeId id, const FaceTextureNode* nodes, FaceTexture* res,
                            ArrayAtlas* atlas)
{
    const FaceTextureNode* n = &nodes[id];
    if (!(n->type & FACETXT_VARIANT_BIT))
    {
        consume_activated(id, nodes, res, atlas);
        return;
    }
    res->flags |= FACETXT_VARIANT_BIT;
    res->variant_count = n->variant.count;
    for (uint8_t i = 0; i < n->variant.count; i++)
        consume_activated(n->variant.children[i], nodes, res, atlas);
}

FaceTexture face_texture_build(const FaceTextureNode* nodes, NodeId root,
                               ArrayAtlas* atlas)
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
