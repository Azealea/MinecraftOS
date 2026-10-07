#pragma once

#include <stdint.h>

typedef struct
{
    uint16_t base_id;
    uint8_t flags;
    uint8_t variant_count;
    uint8_t frame_count;
} FaceTexture;

uint16_t face_texture_resolve(const FaceTexture* f, uint8_t neighbor_mask, bool activated,
                              uint8_t hash_pos);
