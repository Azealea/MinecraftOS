#include "player.h"

#include "app.h"
#include "raycast.h"
#include "utils/vec.h"
#include "voxel/block.h"
#include "voxel/chunk.h"
#include "voxel/world.h"

static void break_on_hit(World* world, RaycastHit hit)
{
    VEC3(u8)
    local = {hit.b.x % CHUNK_SIZE, hit.b.y % CHUNK_SIZE, hit.b.z % CHUNK_SIZE};
    if (!chunk_is_in_bound(local))
        return;
    Chunk* e = world_get_or_add_chunk(world,
                                      ((ChunkPos){.x = hit.b.x / CHUNK_SIZE,
                                                  .y = hit.b.y / CHUNK_SIZE,
                                                  .z = hit.b.z / CHUNK_SIZE}));
    chunk_set(e, local, ((Block){.type = BLK_AIR}));
}

static void place_on_hit(World* world, RaycastHit hit, Block block)
{
    u32 px = hit.b.x + hit.n.x;
    u32 py = hit.b.y + hit.n.y;
    u32 pz = hit.b.z + hit.n.z;
    VEC3(u8) local = {px % CHUNK_SIZE, py % CHUNK_SIZE, pz % CHUNK_SIZE};
    if (!chunk_is_in_bound(local))
        return;
    Chunk* e = world_get_or_add_chunk(
        world,
        ((ChunkPos){.x = px / CHUNK_SIZE, .y = py / CHUNK_SIZE, .z = pz / CHUNK_SIZE}));
    chunk_set(e, local, block);
}

void player_consume_input(App* app)
{
    if (input_mouse_pressed(&app->inputState, 0))
    {
        RaycastHit hit = raycast(&app->camera, &app->world, 8.0f);
        if (hit.hit)
            break_on_hit(&app->world, hit);
    }
    if (input_mouse_pressed(&app->inputState, 1))
    {
        RaycastHit hit = raycast(&app->camera, &app->world, 8.0f);
        if (hit.hit)
            place_on_hit(&app->world, hit, (Block){.type = BLK_DIRT});
    }
}
