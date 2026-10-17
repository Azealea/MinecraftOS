#include <stddef.h>

#include "voxel/block/entities.h"
#include "voxel/world/chunk.h"

static bool is_network_type_block(const Block* b)
{
    return b->type == BLK_APP_CONTROLLER || b->type == BLK_LAMP;
}

void dfs_for_devices_and_source(VEC3(u8) start, Chunk* c,
                                bool visited[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE])
{
    VEC3(u8) found_devices[CHUNK_NB_BLOCKS];
    size_t found_devices_size = 0;

    VEC3(u8) found_sources[CHUNK_NB_BLOCKS];
    size_t found_sources_size = 0;

    VEC3(u8) stack[CHUNK_NB_BLOCKS];
    size_t stack_size = 0;

    visited[start.x][start.y][start.z] = true;
    stack[stack_size++] = start;

    while (stack_size > 0)
    {
        VEC3(u8) pos = stack[--stack_size];
        const Block* curr = chunk_get(c, pos);

        if (curr->type == BLK_APP_CONTROLLER)
        {
            found_sources[found_sources_size++] = pos;
        }
        else
        {
            found_devices[found_devices_size++] = pos;
        }

        for (int i = 0; i < 6; i++)
        {
            VEC3(u8) npos = VEC3_ADD(pos, BLOCK_NEIGHB_DIRS[i]);

            if (!chunk_is_in_bound(npos))
                continue;

            const Block* nghbr = chunk_get(c, npos);
            if (!visited[npos.x][npos.y][npos.z] && is_network_type_block(nghbr))
            {
                visited[npos.x][npos.y][npos.z] = true;
                stack[stack_size++] = npos;
            }
        }
    }

    // TODO: coalescing multiple source, rn choosing the first
    u16 root_entid = 0;
    if (found_sources_size > 0)
    {
        root_entid = chunk_get(c, found_sources[0])->entity_id;
    }

    // updating all devices found
    for (size_t i = 0; i < found_devices_size; i++)
    {
        Block* dev = chunk_get(c, found_devices[i]);
        dev->entity_id = root_entid;
        dev->state = root_entid;
    }
}

static void network_on_place(Block* b, Chunk* c)
{
    bool visited[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE] = {0};

    dfs_for_devices_and_source(chunk_pos_of(c, b), c, visited);
}

static void network_on_break(Block* b, Chunk* c)
{
    VEC3(u8) pos = chunk_pos_of(c, b);

    bool visited[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE] = {0};
    visited[pos.x][pos.y][pos.z] = true;

    for (int i = 0; i < 6; i++)
    {
        VEC3(u8) npos = VEC3_ADD(pos, BLOCK_NEIGHB_DIRS[i]);
        if (!chunk_is_in_bound(npos))
            continue;

        if (visited[npos.x][npos.y][npos.z])
            continue;

        const Block* nghbr = chunk_get(c, npos);
        if (!is_network_type_block(nghbr))
            continue;

        dfs_for_devices_and_source(npos, c, visited);
    }
}

static EntityData* checks_new(Block* b, Chunk* c, enum BlockType type_required)
{
    DEBUG("on_place of %s", blockBehaviours[b->type].str_repr);
    ASSERT(b && b->type == type_required, "not good block type, got %s",
           blockBehaviours[b->type].str_repr);

    return chunk_acquire_new_entity_slot(c, &b->entity_id, b->type);
}

static EntityData* checks_break(Block* b, Chunk* c, enum BlockType type_required)
{
    DEBUG("on_break of %s", blockBehaviours[b->type].str_repr);
    ASSERT(b && b->type == type_required, "not good block type, got %s",
           blockBehaviours[b->type].str_repr);

    return chunk_get_entity(c, b->entity_id);
}

void app_controller_on_place(Block* b, Chunk* c)
{
    checks_new(b, c, BLK_APP_CONTROLLER);
    network_on_place(b, c);
}

void app_controller_on_break(Block* b, Chunk* c)
{
    checks_break(b, c, BLK_APP_CONTROLLER);
    network_on_break(b, c);
    chunk_release_entity_slot(c, b->entity_id);
}

void lamp_on_place(Block* b, Chunk* c)
{
    network_on_place(b, c);
}

void lamp_on_break(Block* b, Chunk* c)
{
    network_on_break(b, c);
}
