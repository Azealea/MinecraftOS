
#include "block.h"

const BlockBehaviour blockBehaviours[BLOCK_COUNT] = {
#define XBLOCK_FN(NAME, ...) [NAME] = {__VA_ARGS__},
    XBLOCK(XBLOCK_FN)
#undef XBLOCK_FN
};
