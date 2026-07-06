#include "camera.h"

#include <cglm/affine.h>
#include <cglm/cam.h>
#include <string.h>

#include "app.h"
#include "camera_ubo.h"
#include "input.h"
#include "vk/buffer/buffer.h"
#include "vk/gpu_resources.h"

static vec3 worldup = {0.0f, 0.0f, 1.0f};

void update_camera_pos(Camera* camera, const Input* input)
{
    if (input->meta)
        return;

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

    app->renderer.buffers.uniforms = calloc(app->maxFramesInFlight, sizeof(GpuBuffer));
    app->renderer.buffers.uniformsMapped = calloc(app->maxFramesInFlight, sizeof(void*));

    for (size_t i = 0; i < app->maxFramesInFlight; i++)
    {
        gpu_buffer_create(app, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                              | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                          &app->renderer.buffers.uniforms[i]);

        vkMapMemory(app->renderer.context.device, app->renderer.buffers.uniforms[i].mem,
                    0, bufferSize, 0, &app->renderer.buffers.uniformsMapped[i]);
    }
}

void destroy_uniform_buffers(App* app)
{
    for (size_t i = 0; i < app->maxFramesInFlight; i++)
    {
        gpu_buffer_destroy(app, &app->renderer.buffers.uniforms[i]);
    }
    free(app->renderer.buffers.uniformsMapped);
    free(app->renderer.buffers.uniforms);
}

void update_uniform_buffer(App* app, uint32_t currentImage)
{
    struct UniformBufferObject ubo = {0};

    glm_mat4_identity(ubo.model);

    vec3 look_pos;
    glm_vec3_add(app->camera.pos, app->camera.basis.front, look_pos);

    glm_lookat(app->camera.pos, look_pos, worldup, ubo.view);

    glm_perspective(glm_rad(45.0f),
                    (float)app->renderer.swapchain.extent.width
                        / (float)app->renderer.swapchain.extent.height,
                    0.1f, 100.0f, ubo.proj);

    ubo.proj[1][1] *= -1;

    memcpy(app->renderer.buffers.uniformsMapped[currentImage], &ubo, sizeof(ubo));
}

Camera camera_init(void)
{
    Camera camera;

    glm_vec3_copy((vec3){0.0f, -3.0f, 1.0f}, camera.pos);
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
