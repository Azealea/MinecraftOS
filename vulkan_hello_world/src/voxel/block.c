#include "block.h"

const BlockBehaviour blockBehaviours[BLOCK_COUNT] = {
#define XBLOCK_FN(NAME, ...) [NAME] = {.str_repr = #NAME, __VA_ARGS__},
    XBLOCK(XBLOCK_FN)
#undef XBLOCK_FN
};

const VEC3(i8) BLOCK_NEIGHB_DIRS[6] = {
    {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1},
};
