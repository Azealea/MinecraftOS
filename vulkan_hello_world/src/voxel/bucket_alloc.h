#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "voxel/chunk.h"
#include "voxel/face.h"

#define BUCKET_COUNT (8 * 8 * 8)
#define BUCKET_FACE_CAP (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 6)
#define NO_BUCKET UINT32_MAX

typedef struct
{
    Face* faces;
    bool free[BUCKET_COUNT];
} BucketAlloc;

BucketAlloc bucket_alloc_init(void);
void bucket_alloc_free(BucketAlloc* ba);
uint32_t bucket_alloc_acquire(BucketAlloc* ba);
void bucket_alloc_release(BucketAlloc* ba, uint32_t slot);

static inline Face* bucket_ptr(const BucketAlloc* ba, uint32_t slot)
{
    return ba->faces + (size_t)slot * BUCKET_FACE_CAP;
}
