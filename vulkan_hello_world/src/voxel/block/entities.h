#pragma once

#include "utils/container.h"

typedef struct EntityData
{
    char* msg;
} EntityData;

typedef EntityData* EntityData_ptr;

VECTOR_DECLARE(EntityData_ptr);
