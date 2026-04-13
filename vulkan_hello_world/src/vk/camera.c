#include "camera.h"

#include <cglm/affine.h>
#include <cglm/cam.h>
#include <string.h>

#include "app.h"
#include "camera_ubo.h"
#include "input.h"
#include "vk/buffer/buffer.h"

static vec3 worldup = {0.0f, 0.0f, 1.0f};

void update_camera_pos(Camera* camera, const Input* input)
{
    if (input_key_pressed(input, KEY_TAB))
        camera->fast = !camera->fast;

    float movescale = 0.0002f;
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

void create_uniform_buffers(App* app)
{
    VkDeviceSize bufferSize = sizeof(struct UniformBufferObject);

    app->uniformBuffers =
        calloc(app->maxFramesInFlight, sizeof(*app->uniformBuffers));
    app->uniformBuffersMemory =
        calloc(app->maxFramesInFlight, sizeof(*app->uniformBuffersMemory));
    app->uniformBuffersMapped =
        calloc(app->maxFramesInFlight, sizeof(*app->uniformBuffersMapped));

    for (size_t i = 0; i < app->maxFramesInFlight; i++)
    {
        create_buffer(app, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                          | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      &app->uniformBuffers[i], &app->uniformBuffersMemory[i]);

        vkMapMemory(app->context.device, app->uniformBuffersMemory[i], 0,
                    bufferSize, 0, &app->uniformBuffersMapped[i]);
    }
}

void destroy_uniform_buffers(App* app)
{
    for (size_t i = 0; i < app->maxFramesInFlight; i++)
    {
        vkDestroyBuffer(app->context.device, app->uniformBuffers[i],
                        app->allocator);
        vkFreeMemory(app->context.device, app->uniformBuffersMemory[i],
                     app->allocator);
    }
    free(app->uniformBuffersMapped);
    free(app->uniformBuffersMemory);
    free(app->uniformBuffers);
}

void update_uniform_buffer(App* app, uint32_t currentImage)
{
    struct UniformBufferObject ubo = {0};

    glm_mat4_identity(ubo.model);

    vec3 look_pos;
    glm_vec3_add(app->camera.pos, app->camera.basis.front, look_pos);

    glm_lookat(app->camera.pos, look_pos, worldup, ubo.view);

    glm_perspective(glm_rad(45.0f),
                    (float)app->swapchain.imageExtent.width
                        / (float)app->swapchain.imageExtent.height,
                    0.1f, 100.0f, ubo.proj);

    ubo.proj[1][1] *= -1;

    memcpy(app->uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

Camera camera_init(void)
{
    Camera camera;

    glm_vec3_copy((vec3){0.0f, -3.0f, 1.0f}, camera.pos);
    camera.yaw = glm_rad(90.0f);
    camera.pitch = 0.0f;

    camera.firstMouse = true;
    camera.lastX = 0.0;
    camera.lastY = 0.0;

    vec3 front = {cosf(camera.yaw) * cosf(camera.pitch),
                  sinf(camera.yaw) * cosf(camera.pitch), sinf(camera.pitch)};
    glm_normalize(front);
    glm_vec3_copy(front, camera.basis.front);

    // todo give proper worldup at least a variable or something
    static vec3 worldup = {0.0f, 0.0f, 1.0f};
    glm_vec3_cross(camera.basis.front, worldup, camera.basis.right);
    glm_normalize(camera.basis.right);

    glm_vec3_cross(camera.basis.right, camera.basis.front, camera.basis.up);

    return camera;
}
