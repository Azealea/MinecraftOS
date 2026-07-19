#include "camera_ubo.h"

#include <cglm/affine.h>
#include <cglm/cam.h>
#include <string.h>

#include "app.h"
#include "vk/buffer/buffer.h"
#include "vk/gpu_resources.h"

static vec3 worldup = {0.0f, 0.0f, 1.0f};

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

    Camera* cam = &app->player.cam;

    vec3 look_pos;
    glm_vec3_add(cam->pos, cam->basis.front, look_pos);

    glm_lookat(cam->pos, look_pos, worldup, ubo.view);

    glm_perspective(glm_rad(45.0f),
                    (float)app->renderer.swapchain.extent.width
                        / (float)app->renderer.swapchain.extent.height,
                    0.1f, 100.0f, ubo.proj);

    ubo.proj[1][1] *= -1;

    memcpy(app->renderer.buffers.uniformsMapped[currentImage], &ubo, sizeof(ubo));
}
