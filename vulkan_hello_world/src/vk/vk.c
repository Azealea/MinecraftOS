#include "vk/vk.h"

#include "vk/buffer/cam.h"
#include "vk/buffer/vertex.h"
#include "vk/renderer/vk_command.h"
#include "vk/renderer/vk_despcriptor.h"
#include "vk/renderer/vk_renderer.h"
#include "vk/vk_context.h"
#include "vk/vk_swapchain.h"

void init_vk(App* app)
{
    create_vk_context(app);
    create_swapchain(app);
    create_descriptor_set_layout(app);
    create_renderer(app);
    create_vertex_buffer(app);
    create_index_buffer(app);
    create_uniform_buffers(app);
    create_descriptor_pool(app);
    create_descriptor_sets(app);
}

void clean_vk(App* app)
{
    vkQueueWaitIdle(app->context.queue);
    destroy_renderer(app);
    destroy_swapchain(app);
    destroy_uniform_buffers(app);
    destroy_descriptor_pool(app);
    destroy_descriptor_set_layout(app);
    destroy_index_buffer(app);
    destroy_vertex_buffer(app);
    destroy_vk_context(app);
}

void drawFrame(App* app)
{
    static uint32_t currentFrame = 0;

    VkDevice device = app->context.device;

    ASSERT(vkWaitForFences(device, 1,
                           &app->renderer.inFlightFences[currentFrame], VK_TRUE,
                           UINT64_MAX)
               == VK_SUCCESS,
           "Failed to wait for in-flight fence for frame %u", currentFrame);

    uint32_t imageIndex = acquire_swapchain_image(app, currentFrame);

    ASSERT(vkResetFences(device, 1, &app->renderer.inFlightFences[currentFrame])
               == VK_SUCCESS,
           "Failed to reset in-flight fence for frame %u", currentFrame);

    update_uniform_buffer(app, currentFrame);

    record_command_buffer(app, imageIndex, currentFrame);
    submit_command_buffer(app, imageIndex, currentFrame);
    present_swapchain_image(app, imageIndex);

    currentFrame = (currentFrame + 1) % app->maxFramesInFlight;
}

void do_stuff_vk(App* app)
{
    drawFrame(app);
}
