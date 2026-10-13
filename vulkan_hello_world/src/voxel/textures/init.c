#include "voxel/textures/init.h"

#include "app.h"
#include "vk/vk_texture.h"
#include "voxel/textures/builder/array_atlas.h"
#include "voxel/textures/builder/texture_tree.h"

void debug_print_block_textures(const FaceTexture block_faces[][6])
{
    for (int blk = 0; blk < BLOCK_COUNT; blk++)
    {
        printf("Block %d:\n", blk);
        for (int face = 0; face < 6; face++)
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

void create_texture_stuff(App* app)
{
    ASSERT(validate_block_textures(), "block texture table is incomplete");

    VECTOR(texture_t) atlas = VECTOR_INIT_CAP(texture_t, MAX_TEXTURE);
    build_texture_look_up(app->mesher.Block2TextureMap, &atlas);
    //    debug_print_block_textures(app->mesher.Block2TextureMap);
    create_texture_image_from_atlas(app, &atlas);
    create_texture_sampler(app);
    atlas_clean(&atlas);
}
