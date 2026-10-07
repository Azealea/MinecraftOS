#include "connected_texture.h"

uint8_t connected_mask_pack(bool u0, bool u1, bool v0, bool v1, bool nw, bool ne, bool sw,
                            bool se)
{
    uint8_t mask = 0;
    if (u0)
        mask |= CONNECTED4_U0;
    if (u1)
        mask |= CONNECTED4_U1;
    if (v0)
        mask |= CONNECTED4_V0;
    if (v1)
        mask |= CONNECTED4_V1;
    if (nw)
        mask |= CONNECTED8_NW;
    if (ne)
        mask |= CONNECTED8_NE;
    if (sw)
        mask |= CONNECTED8_SW;
    if (se)
        mask |= CONNECTED8_SE;
    return mask;
}

static const uint8_t CONNECTED8_CASE_OFFSET[CONNECTED4_CASE_COUNT] = {
    0, 1, 2, 3, 4, 5, 7, 9, 13, 14, 16, 18, 22, 23, 27, 31};

uint8_t connected8_case_index(uint8_t mask)
{
    uint8_t edge_mask = mask & 0xF;
    bool u0 = mask & CONNECTED4_U0, u1 = mask & CONNECTED4_U1;
    bool v0 = mask & CONNECTED4_V0, v1 = mask & CONNECTED4_V1;

    uint8_t diag = 0;
    if (u0 && v0)
        diag = (uint8_t)((diag << 1) | ((mask & CONNECTED8_NW) != 0));
    if (u1 && v0)
        diag = (uint8_t)((diag << 1) | ((mask & CONNECTED8_NE) != 0));
    if (u0 && v1)
        diag = (uint8_t)((diag << 1) | ((mask & CONNECTED8_SW) != 0));
    if (u1 && v1)
        diag = (uint8_t)((diag << 1) | ((mask & CONNECTED8_SE) != 0));

    return (uint8_t)(CONNECTED8_CASE_OFFSET[edge_mask] + diag);
}
