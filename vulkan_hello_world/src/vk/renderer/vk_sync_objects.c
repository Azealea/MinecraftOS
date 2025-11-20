#include "vk/renderer/vk_sync_objects.h"

void create_sync_objects(App* app)
{
    ASSERT(
        vkCreateSemaphore(app->context.device,
                          &(VkSemaphoreCreateInfo){
                              .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                          },
                          app->allocator, &app->renderer.imageAcquiredSemaphore)
            == VK_SUCCESS,
        "Couldn't create image acquired semaphore");

    ASSERT(vkCreateSemaphore(
               app->context.device,
               &(VkSemaphoreCreateInfo){
                   .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
               },
               app->allocator, &app->renderer.renderFinishedSemaphore)
               == VK_SUCCESS,
           "Couldn't create render finished semaphore");

    ASSERT(vkCreateFence(app->context.device,
                         &(VkFenceCreateInfo){
                             .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                         },
                         app->allocator, &app->renderer.inFlightFence)
               == VK_SUCCESS,
           "Couldn't create in-flight fence");
}
void destroy_sync_objects(App* app)
{
    vkDestroyFence(app->context.device, app->renderer.inFlightFence,
                   app->allocator);

    vkDestroySemaphore(app->context.device,
                       app->renderer.renderFinishedSemaphore, app->allocator);

    vkDestroySemaphore(app->context.device,
                       app->renderer.imageAcquiredSemaphore, app->allocator);
}