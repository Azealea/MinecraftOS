#include "vk/renderer/vk_sync_objects.h"

#include <stdlib.h>

void create_sync_objects(App* app)
{
    app->renderer.imageAvailableSemaphores =
        malloc(sizeof(VkSemaphore) * app->swapchain.imageCount);
    app->renderer.renderFinishedSemaphores =
        malloc(sizeof(VkSemaphore) * app->swapchain.imageCount);
    app->renderer.inFlightFences =
        malloc(sizeof(VkFence) * app->swapchain.imageCount);

    for (uint32_t i = 0; i < app->swapchain.imageCount; i++)
    {
        ASSERT(vkCreateSemaphore(
                   app->context.device,
                   &(VkSemaphoreCreateInfo){
                       .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                   },
                   app->allocator, &app->renderer.imageAvailableSemaphores[i])
                   == VK_SUCCESS,
               "Couldn't create image acquired semaphore");

        ASSERT(vkCreateSemaphore(
                   app->context.device,
                   &(VkSemaphoreCreateInfo){
                       .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                   },
                   app->allocator, &app->renderer.renderFinishedSemaphores[i])
                   == VK_SUCCESS,
               "Couldn't create render finished semaphore");
        ASSERT(vkCreateFence(app->context.device,
                             &(VkFenceCreateInfo){
                                 .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                                 .flags = VK_FENCE_CREATE_SIGNALED_BIT },
                             app->allocator, &app->renderer.inFlightFences[i])
                   == VK_SUCCESS,
               "Couldn't create in-flight fence");
    }
}
void destroy_sync_objects(App* app)
{
    for (uint32_t i = 0; i < app->swapchain.imageCount; i++)
    {
        vkDestroyFence(app->context.device, app->renderer.inFlightFences[i],
                       app->allocator);
        vkDestroySemaphore(app->context.device,
                           app->renderer.renderFinishedSemaphores[i],
                           app->allocator);

        vkDestroySemaphore(app->context.device,
                           app->renderer.imageAvailableSemaphores[i],
                           app->allocator);
    }
    free(app->renderer.inFlightFences);
    free(app->renderer.imageAvailableSemaphores);
    free(app->renderer.renderFinishedSemaphores);
}