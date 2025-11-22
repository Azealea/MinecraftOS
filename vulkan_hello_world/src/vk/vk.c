#include "vk/vk.h"

#include "vk/renderer/vk_command.h"
#include "vk/renderer/vk_renderer.h"
#include "vk/vk_context.h"
#include "vk/vk_swapchain.h"

void init_vk(App* app)
{
    create_vk_context(app);
    create_swapchain(app);
    create_renderer(app);
}

void clean_vk(App* app)
{
    destroy_renderer(app);
    destroy_swapchain(app);
    destroy_vk_context(app);
}

void do_stuff_vk(App* app)
{
    static uint32_t currentFrame = 0;
    vkWaitForFences(app->context.device, 1,
                    &app->renderer.inFlightFences[currentFrame], VK_TRUE,
                    UINT64_MAX);
    vkResetFences(app->context.device, 1,
                  &app->renderer.inFlightFences[currentFrame]);

    uint32_t acquiredIndex;
    vkAcquireNextImageKHR(app->context.device, app->swapchain.swapchain,
                          UINT64_MAX,
                          app->renderer.imageAvailableSemaphores[currentFrame],
                          VK_NULL_HANDLE, &acquiredIndex);
    // uint32_t acquiredIndex = acquire_swapchain_image(app, currentFrame);
    vkResetCommandBuffer(app->renderer.commandBuffers[acquiredIndex], 0);

    record_command_buffer(app, acquiredIndex);
    submit_command_buffer(app, acquiredIndex, currentFrame);
    present_swapchain_image(app, acquiredIndex, currentFrame);
    currentFrame = (currentFrame + 1) % app->swapchain.imageCount;
}
