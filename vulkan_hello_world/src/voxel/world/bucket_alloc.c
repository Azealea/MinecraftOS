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

Face* bucket_alloc_acquire(BucketAlloc* ba)
{
    for (u32 i = 0; i < BUCKET_COUNT; i++)
    {
        if (ba->free[i])
        {
            ba->free[i] = false;
            return ba->faces + i * BUCKET_FACE_CAP;
        }
    }
    ERROR("BucketAlloc exhausted (%d buckets)", BUCKET_COUNT);
    return NULL;
}

void bucket_alloc_release(BucketAlloc* ba, Face* slot)
{
    u32 index = (slot - ba->faces) / BUCKET_FACE_CAP;

    ba->free[index] = true;
}
