#include "player.h"

#include "app.h"
#include "raycast.h"
#include "utils/log.h"
#include "utils/vec.h"
#include "voxel/block.h"
#include "voxel/world/world.h"

static void break_on_hit(App* app, RaycastHit hit)
{
    world_set_block(&app->world, hit.block, (Block){.type = BLK_AIR});
    TRACE("breaking " VEC3_FMT, VEC3_ARGS(hit.block));
}

static void place_on_hit(App* app, RaycastHit hit, Block block)
{
    VEC3(i32) blockPlace = VEC3_ADD(hit.block, hit.normal);
    world_set_block(&app->world, blockPlace, block);
    TRACE("placing " VEC3_FMT, VEC3_ARGS(blockPlace));
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
