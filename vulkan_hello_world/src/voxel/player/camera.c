#include "camera.h"

static VEC3(f32) worldup = {0.0f, 0.0f, 1.0f};

void update_camera_pos(Camera* camera, const Input* input)
{
    if (input->meta)
        return;

    if (input_key_pressed(input, KEY_TAB))
        camera->fast = !camera->fast;

    float movescale = 0.02f;
    if (camera->fast)
        movescale *= 10.0f;

    if (input_key_held(input, KEY_W))
        camera->pos = VEC3_ADD(camera->pos, VEC3_SCALE(camera->basis.front, movescale));
    if (input_key_held(input, KEY_S))
        camera->pos = VEC3_ADD(camera->pos, VEC3_SCALE(camera->basis.front, -movescale));
    if (input_key_held(input, KEY_A))
        camera->pos = VEC3_ADD(camera->pos, VEC3_SCALE(camera->basis.right, -movescale));
    if (input_key_held(input, KEY_D))
        camera->pos = VEC3_ADD(camera->pos, VEC3_SCALE(camera->basis.right, movescale));
    if (input_key_held(input, KEY_SPACE))
        camera->pos = VEC3_ADD(camera->pos, VEC3_SCALE(worldup, movescale));
    if (input_key_held(input, KEY_LEFT_SHIFT))
        camera->pos = VEC3_ADD(camera->pos, VEC3_SCALE(worldup, -movescale));

    if (camera->firstMouse)
    {
        camera->firstMouse = false;
        return;
    }

    float sensitivity = 0.01f;
    camera->yaw -= (float)input->mouse_dx * sensitivity;
    camera->pitch -= (float)input->mouse_dy * sensitivity;

    if (camera->pitch > deg_to_rad(89.9f))
        camera->pitch = deg_to_rad(89.9f);
    if (camera->pitch < -deg_to_rad(89.9f))
        camera->pitch = -deg_to_rad(89.9f);

    VEC3(f32) front = {cosf(camera->yaw) * cosf(camera->pitch),
                       sinf(camera->yaw) * cosf(camera->pitch), sinf(camera->pitch)};
    camera->basis.front = vec3_normalize(front);
    camera->basis.right = vec3_normalize(VEC3_CROSS(camera->basis.front, worldup));
    camera->basis.up = VEC3_CROSS(camera->basis.right, camera->basis.front);
}

Camera camera_init(VEC3(i32) pos)
{
    TRACE(VEC3_FMT "\n", VEC3_ARGS(pos));
    Camera camera = {
        .pos = {pos.x, pos.y, pos.z},
    };

    camera.yaw = deg_to_rad(90.0f);
    camera.pitch = 0.0f;

    camera.firstMouse = true;

    VEC3(f32) front = {cosf(camera.yaw) * cosf(camera.pitch),
                       sinf(camera.yaw) * cosf(camera.pitch), sinf(camera.pitch)};
    camera.basis.front = vec3_normalize(front);

    camera.basis.right = vec3_normalize(VEC3_CROSS(camera.basis.front, worldup));
    camera.basis.up = VEC3_CROSS(camera.basis.right, camera.basis.front);

    return camera;
}
