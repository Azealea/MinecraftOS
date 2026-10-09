#include "connected_texture.h"

u8 connected8_case_index(u8 mask)
{
    static const u8 CONNECTED8_CASE_OFFSET[CONNECTED4_CASE_COUNT] = {
        0, 1, 2, 3, 4, 5, 7, 9, 13, 14, 16, 18, 22, 23, 27, 31};
    u8 edge_mask = mask & 0xF;
    bool u0 = mask & CONNECTED4_U0, u1 = mask & CONNECTED4_U1;
    bool v0 = mask & CONNECTED4_V0, v1 = mask & CONNECTED4_V1;

    u8 diag = 0;
    if (u0 && v0)
        diag = (u8)((diag << 1) | ((mask & CONNECTED8_NW) != 0));
    if (u1 && v0)
        diag = (u8)((diag << 1) | ((mask & CONNECTED8_NE) != 0));
    if (u0 && v1)
        diag = (u8)((diag << 1) | ((mask & CONNECTED8_SW) != 0));
    if (u1 && v1)
        diag = (u8)((diag << 1) | ((mask & CONNECTED8_SE) != 0));

    return (u8)(CONNECTED8_CASE_OFFSET[edge_mask] + diag);
}
