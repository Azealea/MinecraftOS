#include "player.h"

#include "app.h"
#include "input/input.h"
#include "player_movement.h"
#include "raycast.h"
#include "utils/log.h"
#include "utils/vec.h"
#include "voxel/block.h"
#include "voxel/world/world.h"

Player player_init(VEC3(i32) pos)
{
    return (Player){.cam = camera_init(pos)};
}

static void break_on_hit(App* app, RaycastHit hit)
{
    world_set_block(&app->world, hit.block, (Block){.type = BLK_AIR}, &app->mesher.dirty);
    TRACE("breaking " VEC3_FMT, VEC3_ARGS(hit.block));
}

static void place_on_hit(App* app, RaycastHit hit, Block block)
{
    VEC3(i32) blockPlace = VEC3_ADD(hit.block, hit.normal);
    world_set_block(&app->world, blockPlace, block, &app->mesher.dirty);
    TRACE("placing " VEC3_FMT, VEC3_ARGS(blockPlace));
}

void player_consume_input(App* app)
{
    Player* p = &app->player;
    update_camera_pos(&p->cam, &app->inputState);

    if (app->inputState.meta)
        return;

    if (input_key_pressed(&app->inputState, KEY_1))
    {
        p->chosenBlock = BLK_APP_CONTROLLER;
    }

    if (input_key_pressed(&app->inputState, KEY_2))
    {
    }

    if (input_key_pressed(&app->inputState, KEY_3))
    {
        p->chosenBlock = BLK_GRASS;
    }

    if (input_key_pressed(&app->inputState, KEY_4))
    {
        p->chosenBlock = BLK_STONE;
    }

    if (input_mouse_pressed(&app->inputState, MOUSE_LEFT))
    {
        RaycastHit hit = raycast(&p->cam, &app->world, 8.0f);
        if (hit.hit)
            break_on_hit(app, hit);
    }
    if (input_mouse_pressed(&app->inputState, MOUSE_RIGHT))
    {
        RaycastHit hit = raycast(&p->cam, &app->world, 8.0f);
        if (hit.hit)
            place_on_hit(app, hit, (Block){.type = p->chosenBlock});
    }
}
