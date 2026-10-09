#pragma once

#include "utils/utils.h"

typedef struct
{
    u16 base_id;
    u8 flags;
    u8 variant_count;
    u8 frame_count;
} FaceTexture;

u16 face_texture_to_index(const FaceTexture* f, u8 neighbor_mask, bool activated,
                          u8 hash_pos);
