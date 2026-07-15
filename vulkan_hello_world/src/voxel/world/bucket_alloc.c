#include "bucket_alloc.h"

#include <stdlib.h>

#include "utils/log.h"

BucketAlloc bucket_alloc_init(void)
{
    BucketAlloc ba = {
        .faces = malloc((size_t)BUCKET_COUNT * BUCKET_FACE_CAP * sizeof(Face)),
    };
    for (int i = 0; i < BUCKET_COUNT; i++)
        ba.free[i] = true;
    return ba;
}

void bucket_alloc_free(BucketAlloc* ba)
{
    free(ba->faces);
}

uint32_t bucket_alloc_acquire(BucketAlloc* ba)
{
    for (uint32_t i = 0; i < BUCKET_COUNT; i++)
    {
        if (ba->free[i])
        {
            ba->free[i] = false;
            return i;
        }
    }
    ERROR("BucketAlloc exhausted (%d buckets)", BUCKET_COUNT);
    return NO_BUCKET;
}

void bucket_alloc_release(BucketAlloc* ba, uint32_t slot)
{
    ba->free[slot] = true;
}
