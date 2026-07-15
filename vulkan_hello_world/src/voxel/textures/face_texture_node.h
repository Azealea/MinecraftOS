#pragma once

#include <stdint.h>

#define BLOCK_FACE_COUNT 6

typedef uint16_t NodeId;
#define FTN_VARIANT_MAX_CHILDREN 8

typedef enum : uint8_t
{
    FACETXT_CONNECTED4_BIT = 1 << 0,
    FACETXT_CONNECTED8_BIT = 1 << 1,
    FACETXT_ACTIVATED_BIT = 1 << 2,
    FACETXT_VARIANT_BIT = 1 << 3,
} FaceTextureFlags;

typedef struct FaceTextureNode
{
    FaceTextureFlags type; // 0 = leaf
    union
    {
        struct
        {
            uint32_t texture_id;
            uint8_t count;
        } leaf;
        struct
        {
            NodeId inner, edge, corner;
        } connected;
        struct
        {
            NodeId off, on;
        } activated;
        struct
        {
            NodeId children[FTN_VARIANT_MAX_CHILDREN];
            uint8_t count;
        } variant;
    };
} FaceTextureNode;
