#pragma once

#include <math.h>
#include <string.h>

#include "utils/utils.h"

typedef float mat4[4][4];

static inline float deg_to_rad(float deg)
{
    // pies ? we love em
    return deg * 3.14159265358979323846f / 180.0f;
}

static inline VEC3(f32) vec3_normalize(VEC3(f32) v)
{
    float len = sqrtf(VEC3_DOT(v, v));
    if (len < 1e-6f)
        return (VEC3(f32)){0};
    return VEC3_SCALE(v, 1.0f / len);
}

static inline void mat4_identity(mat4 dest)
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            dest[i][j] = (i == j) ? 1.0f : 0.0f;
}

static inline void mat4_lookat(VEC3(f32) eye, VEC3(f32) center, VEC3(f32) up, mat4 dest)
{
    VEC3(f32) f = vec3_normalize(VEC3_SUB(center, eye));
    VEC3(f32) s = vec3_normalize(VEC3_CROSS(f, up));
    VEC3(f32) u = VEC3_CROSS(s, f);

    dest[0][0] = s.x;
    dest[0][1] = u.x;
    dest[0][2] = -f.x;
    dest[0][3] = 0.0f;

    dest[1][0] = s.y;
    dest[1][1] = u.y;
    dest[1][2] = -f.y;
    dest[1][3] = 0.0f;

    dest[2][0] = s.z;
    dest[2][1] = u.z;
    dest[2][2] = -f.z;
    dest[2][3] = 0.0f;

    dest[3][0] = -VEC3_DOT(s, eye);
    dest[3][1] = -VEC3_DOT(u, eye);
    dest[3][2] = VEC3_DOT(f, eye);
    dest[3][3] = 1.0f;
}

static inline void mat4_perspective(float fovy, float aspect, float nearZ, float farZ,
                                    mat4 dest)
{
    memset(dest, 0, sizeof(mat4));

    float f = 1.0f / tanf(fovy * 0.5f);
    float fn = 1.0f / (nearZ - farZ);

    dest[0][0] = f / aspect;
    dest[1][1] = f;
    dest[2][2] = (nearZ + farZ) * fn;
    dest[2][3] = -1.0f;
    dest[3][2] = 2.0f * nearZ * farZ * fn;
}

static inline void mat4_mul(mat4 a, mat4 b, mat4 dest)
{
    for (int col = 0; col < 4; col++)
        for (int row = 0; row < 4; row++)
        {
            float sum = 0.0f;
            for (int k = 0; k < 4; k++)
                sum += a[k][row] * b[col][k];
            dest[col][row] = sum;
        }
}
