#include "face_texture.h"

#include "voxel/textures/connected_texture.h"
#include "voxel/textures/texture_flags.h"

u16 face_texture_to_index(const FaceTexture* f, u8 neighbor_mask, bool activated,
                          u8 hash_pos)
{
    const u16 connected_size = (f->flags & BLKTXT_CONNECTED8_BIT) ? CONNECTED8_CASE_COUNT
        : (f->flags & BLKTXT_CONNECTED4_BIT)                      ? CONNECTED4_CASE_COUNT
                                                                  : 1;
    const u8 activated_size = (f->flags & BLKTXT_ACTIVATED_BIT) ? 2 : 1;

    const u8 variant_pos =
        (f->flags & BLKTXT_VARIANT_BIT) ? hash_pos % f->variant_count : 0;
    const u8 activated_pos = (f->flags & BLKTXT_ACTIVATED_BIT) ? (activated ? 1 : 0) : 0;
    const u16 connected_pos = (f->flags & BLKTXT_CONNECTED8_BIT)
        ? connected8_case_index(neighbor_mask)
        : (f->flags & BLKTXT_CONNECTED4_BIT) ? (neighbor_mask & 0xF)
                                             : 0;

    int res = f->base_id
        + ((u32)variant_pos * activated_size * connected_size
           + (u32)activated_pos * connected_size + (u32)connected_pos)
            * f->frame_count;
    return res;
}
