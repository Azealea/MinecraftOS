#include "player.h"

#include "app.h"
#include "raycast.h"
#include "utils/log.h"
#include "utils/vec.h"
#include "voxel/block.h"
#include "voxel/chunk.h"
#include "voxel/world.h"

static void trigger_mesh_rebuild(World* w, Chunk* c)
{}

static void break_on_hit(World* w, RaycastHit hit)
{
    Chunk* c = world_get_or_add_chunk(w, pos_glob_to_chunk(hit.block));

    VEC3(u8) local = pos_glob_to_rel(hit.block);
    chunk_set(c, local, ((Block){.type = BLK_AIR}));

    TRACE("breaking " VEC3_FMT, VEC3_ARGS(local));
    trigger_mesh_rebuild(w, c);
}

static void place_on_hit(World* w, RaycastHit hit, Block block)
{
    VEC3(i32) blockPlace = VEC3_ADD(hit.block, hit.normal);

    Chunk* c = world_get_or_add_chunk(w, pos_glob_to_chunk(blockPlace));

    VEC3(u8) local = pos_glob_to_rel(blockPlace);
    chunk_set(c, local, block);

    TRACE("placing " VEC3_FMT, VEC3_ARGS(local));
    trigger_mesh_rebuild(w, c);
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
