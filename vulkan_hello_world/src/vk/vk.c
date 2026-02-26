#include "vk/vk.h"

#include "vk/buffer/cam.h"
#include "vk/buffer/vertex.h"
#include "vk/renderer/vk_command.h"
#include "vk/renderer/vk_despcriptor.h"
#include "vk/renderer/vk_graphics_pipeline.h"
#include "vk/renderer/vk_sync_objects.h"
#include "vk/vk_context.h"
#include "vk/vk_depth.h"
#include "vk/vk_swapchain.h"
#include "vk/vk_texture.h"

void init_vk(App* app)
{
    create_vk_context(app);
    create_swapchain(app);
    create_renderpass(app);
    create_descriptor_set_layout(app);
    create_graphics_pipeline(app);
    create_command_pool(app);
    create_depth_resources(app);
    create_framebuffers(app);
    create_texture_image(app);
    create_texture_image_view(app);
    create_texture_sampler(app);
    create_vertex_buffer(app);
    create_index_buffer(app);
    create_uniform_buffers(app);
    create_descriptor_pool(app);
    create_descriptor_sets(app);
    allocate_command_buffer(app);
    create_sync_objects(app);
}

void clean_vk(App* app)
{
    vkDeviceWaitIdle(app->context.device);
    destroy_depth_resources(app);
    destroy_framebuffers(app);
    destroy_swapchain(app);
    destroy_graphics_pipeline(app);
    destroy_renderpass(app);
    destroy_uniform_buffers(app);
    destroy_descriptor_pool(app);
    destroy_texture_sampler(app);
    destroy_texture_image_view(app);
    destroy_texture_image(app);
    destroy_descriptor_set_layout(app);
    destroy_index_buffer(app);
    destroy_vertex_buffer(app);
    destroy_sync_objects(app);
    destroy_command_pool(app);
    destroy_vk_context(app);
}

void drawFrame(App* app)
{
    static uint32_t currentFrame = 0;

    VkDevice device = app->context.device;

    ASSERTVK(vkWaitForFences(device, 1,
                             &app->renderer.inFlightFences[currentFrame],
                             VK_TRUE, UINT64_MAX),
             "Failed to wait for in-flight fence for frame %u", currentFrame);

    uint32_t imageIndex = acquire_swapchain_image(app, currentFrame);

    ASSERTVK(
        vkResetFences(device, 1, &app->renderer.inFlightFences[currentFrame]),
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
