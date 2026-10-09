#include "connected_texture_build.h"

#include <stdlib.h>
#include <string.h>

#include "voxel/textures/builder/array_atlas.h"
#include "voxel/textures/connected_texture.h"

#define QUAD_SIZE (TEXTURE_WIDTH_HEIGHT / 2)

enum
{
    QUAD_CORNER = 0,
    QUAD_OVERFLOW = 1,
    QUAD_EDGE = 2,
    QUAD_NULL = 3,
};

static void blit_quadrant(texture_t out, texture_t border, int quad, int dst_x, int dst_y,
                          bool target_flip_x, bool target_flip_y, bool transpose)
{
    int qx = (quad % 2) * QUAD_SIZE;
    int qy = (quad / 2) * QUAD_SIZE;

    bool flip_x = (quad == QUAD_OVERFLOW) ? !target_flip_x : target_flip_x;
    bool flip_y = target_flip_y;
    bool flip_sx = transpose ? flip_y : flip_x;
    bool flip_sy = transpose ? flip_x : flip_y;

    for (int y = 0; y < QUAD_SIZE; y++)
        for (int x = 0; x < QUAD_SIZE; x++)
        {
            int sx = transpose ? y : x;
            int sy = transpose ? x : y;
            if (flip_sx)
                sx = QUAD_SIZE - 1 - sx;
            if (flip_sy)
                sy = QUAD_SIZE - 1 - sy;

            size_t si =
                ((size_t)(qy + sy) * TEXTURE_WIDTH_HEIGHT + (size_t)(qx + sx)) * 4;
            size_t di =
                ((size_t)(dst_y + y) * TEXTURE_WIDTH_HEIGHT + (size_t)(dst_x + x)) * 4;

            uint8_t a = border[si + 3];
            for (int c = 0; c < 3; c++)
                out[di + c] =
                    (uint8_t)(((int)border[si + c] * a + (int)out[di + c] * (255 - a))
                              / 255);
            out[di + 3] = out[di + 3] > a ? out[di + 3] : a;
        }
}

static void composite_connected_tile(VECTOR(texture_t)* atlas, texture_t inner,
                                     texture_t border, uint16_t mask, bool has_diagonal)
{
    texture_t out = malloc((size_t)TEXTURE_WIDTH_HEIGHT * TEXTURE_WIDTH_HEIGHT * 4);
    memcpy(out, inner, (size_t)TEXTURE_WIDTH_HEIGHT * TEXTURE_WIDTH_HEIGHT * 4);

    bool u0 = mask & CONNECTED4_U0, u1 = mask & CONNECTED4_U1;
    bool v0 = mask & CONNECTED4_V0, v1 = mask & CONNECTED4_V1;

    const struct
    {
        int dx, dy;
        bool edge_a; // west for NW/SW, east for NE/SE
        bool edge_b; // north for NW/NE, south for SW/SE
        bool diag; // only meaningful when has_diagonal
        bool flip_x, flip_y;
    } corners[4] = {
        {0, 0, u0, v0, mask & CONNECTED8_NW, false, false},
        {QUAD_SIZE, 0, u1, v0, mask & CONNECTED8_NE, true, false},
        {0, QUAD_SIZE, u0, v1, mask & CONNECTED8_SW, false, true},
        {QUAD_SIZE, QUAD_SIZE, u1, v1, mask & CONNECTED8_SE, true, true},
    };

    for (int i = 0; i < 4; i++)
    {
        bool transpose = false;
        int quad;

        if (!corners[i].edge_a && !corners[i].edge_b)
            quad = QUAD_CORNER;
        else if (corners[i].edge_a && corners[i].edge_b)
            quad = (has_diagonal && !corners[i].diag) ? QUAD_OVERFLOW : QUAD_NULL;
        else if (corners[i].edge_a)
        {
            quad = QUAD_EDGE;
            transpose = true;
        }
        else
            quad = QUAD_EDGE;

        blit_quadrant(out, border, quad, corners[i].dx, corners[i].dy, corners[i].flip_x,
                      corners[i].flip_y, transpose);
    }

    atlas_push_pixels(atlas, out);
}

void bake_connected4(VECTOR(texture_t)* atlas, texture_t inner_px, texture_t border_px)
{
    for (uint16_t mask = 0; mask < CONNECTED4_CASE_COUNT; mask++)
        composite_connected_tile(atlas, inner_px, border_px, mask, false);
}

void bake_connected8(VECTOR(texture_t)* atlas, texture_t inner_px, texture_t border_px)
{
    uint16_t representative[CONNECTED8_CASE_COUNT];
    bool seen[CONNECTED8_CASE_COUNT] = {false};
    for (uint16_t mask = 0; mask < 256; mask++)
    {
        uint8_t idx = connected8_case_index((uint8_t)mask);
        if (!seen[idx])
        {
            seen[idx] = true;
            representative[idx] = mask;
        }
    }

    for (int idx = 0; idx < CONNECTED8_CASE_COUNT; idx++)
        composite_connected_tile(atlas, inner_px, border_px, representative[idx], true);
}
