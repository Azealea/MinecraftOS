#include <stdio.h>

#include "block.h"
#include "textures/face_texture.h"

static void
build_texture_look_up(const FaceTextureBuilder builders[][FACE_COUNT],
                      FaceTexture destination[][FACE_COUNT], ArrayAtlas* atlas)
{
    for (int b = 0; b < BlockFaceBuildersCount; b++)
        for (int f = 0; f < FACE_COUNT; f++)
            destination[b][f] = face_texture_build(&builders[b][f], atlas);
}

void load_texture_into_atlas(ArrayAtlas* atlas)
{
    build_texture_look_up(BlockFaceBuilders, BlockFaces, atlas);
}

void debug_print_block_faces(void)
{
    for (int blk = 0; blk < BLOCK_COUNT; blk++)
    {
        printf("Block %d:\n", blk);
        for (int face = 0; face < FACE_COUNT; face++)
        {
            const FaceTexture* f = &BlockFaces[blk][face];
            printf("  face %d: base_id=%-4u flags=0x%02x variant_count=%-3u "
                   "frame_count=%-3u | "
                   "connected=%s activated=%s variant=%s\n",
                   face, f->base_id, f->flags, f->variant_count, f->frame_count,
                   (f->flags & FACETXT_CONNECTED8_BIT)       ? "8"
                       : (f->flags & FACETXT_CONNECTED4_BIT) ? "4"
                                                             : "-",
                   (f->flags & FACETXT_ACTIVATED_BIT) ? "yes" : "no",
                   (f->flags & FACETXT_VARIANT_BIT) ? "yes" : "no");
        }
    }
}
