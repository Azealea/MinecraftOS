#include "vk/renderer/vk_sync_objects.h"

#include <stdlib.h>

void create_sync_objects(App* app)
{
    app->renderer.sync.imageAvailable =
        malloc(sizeof(VkSemaphore) * app->maxFramesInFlight);
    app->renderer.sync.renderFinished =
        malloc(sizeof(VkSemaphore) * app->renderer.swapchain.imageCount);
    app->renderer.sync.inFlight = malloc(sizeof(VkFence) * app->maxFramesInFlight);

    for (uint32_t i = 0; i < app->maxFramesInFlight; i++)
    {
        ASSERTVK(vkCreateFence(
                     app->renderer.context.device,
                     &(VkFenceCreateInfo){.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                                          .flags = VK_FENCE_CREATE_SIGNALED_BIT},
                     app->renderer.allocator, &app->renderer.sync.inFlight[i]),
                 "Couldn't create in-flight fence");
        ASSERTVK(vkCreateSemaphore(app->renderer.context.device,
                                   &(VkSemaphoreCreateInfo){
                                       .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                                   },
                                   app->renderer.allocator,
                                   &app->renderer.sync.imageAvailable[i]),
                 "Couldn't create image acquired semaphore");
    }

    for (uint32_t i = 0; i < app->renderer.swapchain.imageCount; i++)
    {
        ASSERTVK(vkCreateSemaphore(app->renderer.context.device,
                                   &(VkSemaphoreCreateInfo){
                                       .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                                   },
                                   app->renderer.allocator,
                                   &app->renderer.sync.renderFinished[i]),
                 "Couldn't create render finished semaphore");
    }
}
void destroy_sync_objects(App* app)
{
    for (uint32_t i = 0; i < app->maxFramesInFlight; i++)
    {
        vkDestroyFence(app->renderer.context.device, app->renderer.sync.inFlight[i],
                       app->renderer.allocator);
        vkDestroySemaphore(app->renderer.context.device,
                           app->renderer.sync.imageAvailable[i], app->renderer.allocator);
    }
    for (uint32_t i = 0; i < app->renderer.swapchain.imageCount; i++)
    {
        vkDestroySemaphore(app->renderer.context.device,
                           app->renderer.sync.renderFinished[i], app->renderer.allocator);
    }
    free(app->renderer.sync.inFlight);
    free(app->renderer.sync.imageAvailable);
    free(app->renderer.sync.renderFinished);
}
