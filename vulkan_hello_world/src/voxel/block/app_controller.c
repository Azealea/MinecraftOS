#include "voxel/world/chunk.h"

void app_controller_on_place(Block* b, Chunk* c)
{
    ASSERT(b->type == BLK_APP_CONTROLLER, "not good block type, got %d", b->type);

    DEBUG("on_place");
    EntityData* datat = malloc(sizeof(EntityData));
    datat->msg = "coucou tu veux voir mon beat ?";
    VECTOR_PUSH_BACK(&c->entities, datat);
    b->entity_id = VECTOR_SIZE(&c->entities);
}

void app_controller_on_break(Block* b, Chunk* c)
{
    DEBUG("on_break");
    ASSERT(b->type == BLK_APP_CONTROLLER, "not good block type, got %d", b->type);
    EntityData* data = VECTOR_GET(&c->entities, b->entity_id - 1);
    c->entities.data[b->entity_id - 1] = NULL;

    DEBUG("%s", data->msg);
    free(data);
}
