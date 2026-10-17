#include <string.h>

#include "voxel/world/chunk.h"

// Reminder: vector index != entity_id => right now : entity_id -1 == vector index
// TODO : do a better EntityData alloc : add a free list and kaplamo perfection

EntityData* chunk_acquire_new_entity_slot(Chunk* c, u16* entity_id_ptr,
                                          enum BlockType btype)
{
    VECTOR_PUSH_BACK(&c->entities, (EntityData){.type = btype});

    *entity_id_ptr = c->entities.size;

    return c->entities.data + *entity_id_ptr - 1;
}

EntityData* chunk_get_entity(Chunk* c, u16 entity_id)
{
    if (entity_id == 0)
        return NULL;
    return c->entities.data + entity_id - 1;
}

void chunk_release_entity_slot(Chunk* c, u16 entity_id)
{
    if (entity_id == 0)
        return;
    memset(c->entities.data + entity_id - 1, 0, sizeof(EntityData));
}
