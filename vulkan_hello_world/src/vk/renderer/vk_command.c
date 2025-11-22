#include "vk/renderer/vk_command.h"

#include <stdlib.h>

void create_command_pool(App* app)
{
    ASSERT(vkCreateCommandPool(
               app->context.device,
               &(VkCommandPoolCreateInfo){
                   .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                   .queueFamilyIndex = app->context.queueFamily,
                   .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
               },
               app->allocator, &app->renderer.commandPool)
               == VK_SUCCESS,
           "Couldn't create command pool")
}

void destroy_command_pool(App* app)
{
    vkDestroyCommandPool(app->context.device, app->renderer.commandPool,
                         app->allocator);
}

void allocate_command_buffer(App* app)
{
    app->renderer.commandBuffers =
        malloc(sizeof(VkCommandBuffer) * app->swapchain.imageCount);
    ASSERT(vkAllocateCommandBuffers(
               app->context.device,
               &(VkCommandBufferAllocateInfo){
                   .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                   .commandPool = app->renderer.commandPool,
                   .commandBufferCount = app->swapchain.imageCount,
                   .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
               },
               app->renderer.commandBuffers)
               == VK_SUCCESS,
           "Couldn't allocate command buffer");
}

void destroy_command_buffer(App* app)
{
    free(app->renderer.commandBuffers);
}

void record_command_buffer(App* app, uint32_t imageIndex)
{
    VkCommandBuffer commandBuffer = app->renderer.commandBuffers[imageIndex];

    ASSERT(vkBeginCommandBuffer(
               commandBuffer,
               &(VkCommandBufferBeginInfo){
                   .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
               })
               == VK_SUCCESS,
           "Couldn't begin command buffer for frame");

    vkCmdBeginRenderPass(
        commandBuffer,
        &(VkRenderPassBeginInfo){
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = app->renderer.renderpass,
            .framebuffer = app->renderer.framebuffers[imageIndex],
            .renderArea =
                (VkRect2D){
                    .extent = app->swapchain.imageExtent,
                },
            .clearValueCount = 1,
            .pClearValues = (VkClearValue[]){ app->backgroundColor },

        },
        VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      app->renderer.graphicsPipeline);
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
    vkCmdEndRenderPass(commandBuffer);

    ASSERT(vkEndCommandBuffer(commandBuffer) == VK_SUCCESS,
           "Couldn't end command buffer");
}

void submit_command_buffer(App* app, uint32_t imageIndex, uint32_t frameIndex)
{
    ASSERT(vkQueueSubmit(
               app->context.queue, 1,
               &(VkSubmitInfo){
                   .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                   .commandBufferCount = 1,
                   .pCommandBuffers = &app->renderer.commandBuffers[imageIndex],
                   .waitSemaphoreCount = 1,
                   .pWaitSemaphores =
                       &app->renderer.imageAvailableSemaphores[frameIndex],
                   .signalSemaphoreCount = 1,
                   .pSignalSemaphores =
                       &app->renderer.renderFinishedSemaphores[frameIndex],
                   .pWaitDstStageMask =
                       (VkPipelineStageFlags[]){
                           VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                       } },
               app->renderer.inFlightFences[frameIndex])
               == VK_SUCCESS,
           "Couldn't submit command buffer");
}