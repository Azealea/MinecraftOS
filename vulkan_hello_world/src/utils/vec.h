#pragma once

// non recursive data type for vec2 and vec3 for different size

#define VEC2(TYPE) vec2_##TYPE

#define VEC2_DECLARE(TYPE)                                                     \
    typedef struct VEC2(TYPE)                                                  \
    {                                                                          \
        TYPE x, y;                                                             \
    } VEC2(TYPE)

#define VEC3(TYPE) vec3_##TYPE

#define VEC3_DECLARE(TYPE)                                                     \
    typedef struct VEC3(TYPE)                                                  \
    {                                                                          \
        TYPE x, y, z;                                                          \
    } VEC3(TYPE)

#define VEC2_SUB(A, B)                                                         \
    typeof(A)                                                                  \
    {                                                                          \
        .x = (A).x - (B).x, .y = (A).y - (B).y,                                \
    }

#define VEC2_ADD(A, B)                                                         \
    typeof(A)                                                                  \
    {                                                                          \
        .x = (A).x + (B).x, .y = (A).y + (B).y,                                \
    }

#define VEC2_SCALE(A, K)                                                       \
    typeof(A)                                                                  \
    {                                                                          \
        .x = (A).x * (K), .y = (A).y * (K),                                    \
    }

#define VEC3_EQ(A, B) (A).x == (B).x && (A).y == (B).y && (A).z == (B).z

#define VEC3_SUB(A, B)                                                         \
    ((typeof(A)){                                                              \
        .x = (A).x - (B).x,                                                    \
        .y = (A).y - (B).y,                                                    \
        .z = (A).z - (B).z,                                                    \
    })

#define VEC3_ADD(A, B)                                                         \
    ((typeof(A)){                                                              \
        .x = (A).x + (B).x,                                                    \
        .y = (A).y + (B).y,                                                    \
        .z = (A).z + (B).z,                                                    \
    })

#define VEC3_SCALE(A, K)                                                       \
    ((typeof(A)){                                                              \
        .x = (A).x * (K),                                                      \
        .y = (A).y * (K),                                                      \
        .z = (A).z * (K),                                                      \
    })

#define VEC3_DIV(A, K)                                                         \
    ((typeof(A)){                                                              \
        .x = (A).x / (K),                                                      \
        .y = (A).y / (K),                                                      \
        .z = (A).z / (K),                                                      \
    })

#define VEC3_MOD(A, K)                                                         \
    ((typeof(A)){                                                              \
        .x = (A).x % (K),                                                      \
        .y = (A).y % (K),                                                      \
        .z = (A).z % (K),                                                      \
    })

#define VEC3_CAST(T, V)                                                        \
    ((VEC3(T)){                                                                \
        (T)((V).x),                                                            \
        (T)((V).y),                                                            \
        (T)((V).z),                                                            \
    })

#define VEC3_FOREACH(vec, elem)                                                \
    for (int _i = 0; _i < 3; _i++)                                             \
        for (typeof((vec).data[0]) elem = (vec).data[_i],                      \
                                   *_p = &(vec).data[_i];                      \
             _p; *_p = elem, _p = nullptr)
