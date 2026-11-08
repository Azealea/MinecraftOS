#include "voxel/direction.h"

const VEC3(i8) DIRECTIONS[DIRECTION_COUNT] = {
    [X_PLUS] = {1, 0, 0},   [X_MINUS] = {-1, 0, 0}, [Y_PLUS] = {0, 1, 0},
    [Y_MINUS] = {0, -1, 0}, [Z_PLUS] = {0, 0, 1},   [Z_MINUS] = {0, 0, -1},
};

enum Direction direction_opposite(enum Direction d)
{
    switch (d)
    {
    case X_PLUS:
        return X_MINUS;
    case X_MINUS:
        return X_PLUS;
    case Y_PLUS:
        return Y_MINUS;
    case Y_MINUS:
        return Y_PLUS;
    case Z_PLUS:
        return Z_MINUS;
    case Z_MINUS:
        return Z_PLUS;
    default:
        return d;
    }
}

enum Direction direction_nearest(VEC3(f32) v)
{
    enum Direction res = 0;
    float best_dot = -1000.0f;
    for (enum Direction i = 0; i < DIRECTION_COUNT; i++)
    {
        float dot = VEC3_DOT(v, DIRECTIONS[i]);
        if (dot > best_dot)
        {
            best_dot = dot;
            res = i;
        }
    }
    return res;
}
