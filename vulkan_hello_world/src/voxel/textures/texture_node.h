#pragma once

#include <stdint.h>

typedef uint16_t NodeId;
#define TEXTURE_NODE_VARIANT_MAX_CHILDREN 8

typedef enum : uint8_t
{
    BLKTXT_CONNECTED4_BIT = 1 << 0,
    BLKTXT_CONNECTED8_BIT = 1 << 1,
    BLKTXT_ACTIVATED_BIT = 1 << 2,
    BLKTXT_VARIANT_BIT = 1 << 3,
} TextureFlags;

typedef struct TextureNode
{
    TextureFlags type; // 0 = leaf
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
            NodeId children[TEXTURE_NODE_VARIANT_MAX_CHILDREN];
            uint8_t count;
        } variant;
    };
} TextureNode;
