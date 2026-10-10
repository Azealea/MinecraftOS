#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "voxel/mesh/face.h"
#include "voxel/world/chunk.h"

#define BUCKET_COUNT (8 * 8 * 8)
#define BUCKET_FACE_CAP (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 6)

typedef struct
{
    Face* faces;
    bool free[BUCKET_COUNT];
} BucketAlloc;

BucketAlloc bucket_alloc_init(void);
void bucket_alloc_free(BucketAlloc* ba);

Face* bucket_alloc_acquire(BucketAlloc* ba);
void bucket_alloc_release(BucketAlloc* ba, Face* slot);

u32 bucket_alloc_index_of(const BucketAlloc* ba, const Face* slot);
