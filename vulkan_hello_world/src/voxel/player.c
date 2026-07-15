#include "player.h"

#include "app.h"
#include "raycast.h"
#include "utils/log.h"
#include "utils/vec.h"
#include "vk/vk.h"
#include "voxel/block.h"
#include "voxel/chunk.h"
#include "voxel/world.h"

static void break_on_hit(App* app, RaycastHit hit)
{
    ChunkPos cpos = pos_glob_to_chunk(hit.block);
    Chunk* c = world_get_chunk(&app->world, cpos);

    VEC3(u8) local = pos_glob_to_rel(hit.block);
    chunk_set(c, local, ((Block){.type = BLK_AIR}));

    TRACE("breaking " VEC3_FMT, VEC3_ARGS(local));
    vk_rebuild_mesh(app, cpos);
}

static void place_on_hit(App* app, RaycastHit hit, Block block)
{
    VEC3(i32) blockPlace = VEC3_ADD(hit.block, hit.normal);
    ChunkPos cpos = pos_glob_to_chunk(blockPlace);
    Chunk* c = world_get_or_add_chunk(&app->world, cpos);

    VEC3(u8) local = pos_glob_to_rel(blockPlace);
    chunk_set(c, local, block);

    TRACE("placing " VEC3_FMT, VEC3_ARGS(local));
    vk_rebuild_mesh(app, cpos);
}

void player_consume_input(App* app)
{
    if (app->inputState.meta)
        return;

    if (input_mouse_pressed(&app->inputState, 0))
    {
        RaycastHit hit = raycast(&app->camera, &app->world, 8.0f);
        if (hit.hit)
            break_on_hit(app, hit);
    }
    if (input_mouse_pressed(&app->inputState, 1))
    {
        RaycastHit hit = raycast(&app->camera, &app->world, 8.0f);
        if (hit.hit)
            place_on_hit(app, hit, (Block){.type = BLK_DIRT});
    }
}
