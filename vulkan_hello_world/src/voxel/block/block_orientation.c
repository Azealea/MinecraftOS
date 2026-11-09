#include "voxel/block/block_orientation.h"

enum Direction block_orientation_facing(enum BlockOrientation o)
{
    if (o < BLKORIENT_DOWN_X_PLUS)
        return (enum Direction)o;
    if (o < BLKORIENT_UP_X_PLUS)
        return Z_MINUS;
    return Z_PLUS;
}

enum Direction block_orientation_up(enum BlockOrientation o)
{
    if (o < BLKORIENT_DOWN_X_PLUS)
        return Z_PLUS;
    if (o < BLKORIENT_UP_X_PLUS)
        return (enum Direction)(o - BLKORIENT_DOWN_X_PLUS);
    return (enum Direction)(o - BLKORIENT_UP_X_PLUS);
}
