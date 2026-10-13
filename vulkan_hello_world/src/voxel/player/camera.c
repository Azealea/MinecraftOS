#include "camera.h"

#include <cglm/cam.h>

#include "input/input.h"
#include "utils/utils.h"

static vec3 worldup = {0.0f, 0.0f, 1.0f};

void update_camera_pos(Camera* camera, const Input* input)
{
    if (input->meta)
        return;

    if (input_key_pressed(input, KEY_TAB))
        camera->fast = !camera->fast;

    float movescale = 0.02f;
    if (camera->fast)
        movescale *= 10.0f;

    vec3 delta = {0};

    if (input_key_held(input, KEY_W))
    {
        glm_vec3_scale(camera->basis.front, movescale, delta);
        glm_vec3_add(camera->pos, delta, camera->pos);
    }
    if (input_key_held(input, KEY_S))
    {
        glm_vec3_scale(camera->basis.front, -movescale, delta);
        glm_vec3_add(camera->pos, delta, camera->pos);
    }
    if (input_key_held(input, KEY_A))
    {
        glm_vec3_scale(camera->basis.right, -movescale, delta);
        glm_vec3_add(camera->pos, delta, camera->pos);
    }
    if (input_key_held(input, KEY_D))
    {
        glm_vec3_scale(camera->basis.right, movescale, delta);
        glm_vec3_add(camera->pos, delta, camera->pos);
    }
    if (input_key_held(input, KEY_SPACE))
    {
        glm_vec3_scale(worldup, movescale, delta);
        glm_vec3_add(camera->pos, delta, camera->pos);
    }
    if (input_key_held(input, KEY_LEFT_SHIFT))
    {
        glm_vec3_scale(worldup, -movescale, delta);
        glm_vec3_add(camera->pos, delta, camera->pos);
    }

    if (camera->firstMouse)
    {
        camera->firstMouse = false;
        return;
    }

    float sensitivity = 0.01f;
    camera->yaw -= (float)input->mouse_dx * sensitivity;
    camera->pitch -= (float)input->mouse_dy * sensitivity;

    if (camera->pitch > glm_rad(89.9f))
        camera->pitch = glm_rad(89.9f);
    if (camera->pitch < -glm_rad(89.9f))
        camera->pitch = -glm_rad(89.9f);

    vec3 front = {cosf(camera->yaw) * cosf(camera->pitch),
                  sinf(camera->yaw) * cosf(camera->pitch), sinf(camera->pitch)};
    glm_normalize(front);
    glm_vec3_copy(front, camera->basis.front);
    glm_vec3_cross(camera->basis.front, worldup, camera->basis.right);
    glm_normalize(camera->basis.right);
    glm_vec3_cross(camera->basis.right, camera->basis.front, camera->basis.up);
}

Camera camera_init(VEC3(i32) pos)
{
    TRACE(VEC3_FMT "\n", VEC3_ARGS(pos));
    Camera camera = {
        .pos = {pos.x, pos.y, pos.z},
    };

    camera.yaw = glm_rad(90.0f);
    camera.pitch = 0.0f;

    camera.firstMouse = true;

    vec3 front = {cosf(camera.yaw) * cosf(camera.pitch),
                  sinf(camera.yaw) * cosf(camera.pitch), sinf(camera.pitch)};
    glm_normalize(front);
    glm_vec3_copy(front, camera.basis.front);

    glm_vec3_cross(camera.basis.front, worldup, camera.basis.right);
    glm_normalize(camera.basis.right);

    glm_vec3_cross(camera.basis.right, camera.basis.front, camera.basis.up);

    return camera;
}
