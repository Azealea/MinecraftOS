#include "player.h"

#include "app.h"
#include "input/input.h"
#include "raycast.h"
#include "utils/log.h"
#include "voxel/block.h"

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

static void interact_on_hit(App* app, RaycastHit hit)
{
    world_interact_block(&app->world, hit.block, &app->mesher.dirty);
    TRACE("interacting " VEC3_FMT, VEC3_ARGS(hit.block));
}

void player_consume_input(App* app)
{
    Player* p = &app->player;
    update_camera_pos(&p->cam, &app->inputState);

    if (app->inputState.meta)
        return;

    struct pair_key_block
    {
        InputKey key;
        enum BlockType block;
    } pairs[] = {
        {KEY_0, BLK_AIR},       {KEY_1, BLK_APP_CONTROLLER}, {KEY_2, BLK_APP_BRIDGE},
        {KEY_3, BLK_LAMP},      {KEY_4, BLK_APP_SCREEN},     {KEY_5, BLK_GRASS},
        {KEY_6, BLK_DARKSTONE}, {KEY_7, BLK_SNAKE_TEST},     {KEY_8, BLK_OAK},
    };

    for (size_t i = 0; i < sizeof(pairs) / sizeof(pairs[0]); i++)
        if (input_key_pressed(&app->inputState, pairs[i].key))
        {
            p->chosenBlock = pairs[i].block;
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
        {
            if (p->chosenBlock == BLK_AIR)
            {
                interact_on_hit(app, hit);
            }
            else
            {
                place_on_hit(app, hit, (Block){.type = p->chosenBlock});
            }
        }
    }
}
