#include "cam.h"

#include <cglm/affine.h>
#include <cglm/cam.h>
#include <string.h>

#include "app.h"
#include "utils/time.h"
#include "vk/buffer/buffer.h"

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
    double dt = time_since(app->start_time);

    struct UniformBufferObject ubo = { 0 };
    glm_mat4_identity(ubo.model);

    glm_rotate(ubo.model, dt * glm_rad(90.0f), (vec3){ 0.0f, 0.0f, 1.0f });
    glm_lookat((vec3){ 2.0f, 2.0f, 2.0f }, (vec3){ 0.0f, 0.0f, 0.0f },
               (vec3){ 0.0f, 0.0f, 1.0f }, ubo.view);

    glm_perspective(glm_rad(45.0f),
                    (float)app->swapchain.imageExtent.width
                        / app->swapchain.imageExtent.height,
                    0.1f, 10.0f, ubo.proj);
    ubo.proj[1][1] *= -1;
    memcpy(app->uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}
