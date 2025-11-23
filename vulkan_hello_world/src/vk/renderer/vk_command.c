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
        malloc(sizeof(VkCommandBuffer) * app->maxFramesInFlight);
    ASSERT(vkAllocateCommandBuffers(
               app->context.device,
               &(VkCommandBufferAllocateInfo){
                   .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                   .commandPool = app->renderer.commandPool,
                   .commandBufferCount = app->maxFramesInFlight,
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

void record_command_buffer(App* app, uint32_t imageIndex, uint32_t frameIndex)
{
    VkCommandBuffer commandBuffer = app->renderer.commandBuffers[frameIndex];

    ASSERT(vkResetCommandBuffer(commandBuffer, 0) == VK_SUCCESS,
           "Failed to reset command buffer");

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = 0,
        .pInheritanceInfo = NULL,
    };

    ASSERT(vkBeginCommandBuffer(commandBuffer, &beginInfo) == VK_SUCCESS,
           "Failed to begin recording command buffer");

    VkRenderPassBeginInfo renderPassInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = NULL,
        .renderPass = app->renderer.renderpass,
        .framebuffer = app->renderer.framebuffers[imageIndex],
        .renderArea = {
            .offset = { 0, 0 },
            .extent = app->swapchain.imageExtent,
        },
        .clearValueCount = 1,
        .pClearValues = (VkClearValue[]) { app->backgroundColor},
    };

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      app->renderer.graphicsPipeline);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    ASSERT(vkEndCommandBuffer(commandBuffer) == VK_SUCCESS,
           "Failed to record command buffer for image %u", imageIndex);
}

void submit_command_buffer(App* app, uint32_t imageIndex, uint32_t frameIndex)
{
    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores =
            (VkSemaphore[]){
                app->renderer.imageAvailableSemaphores[frameIndex] },
        .pWaitDstStageMask =
            (VkPipelineStageFlags[]){
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            },
        .commandBufferCount = 1,
        .pCommandBuffers = &app->renderer.commandBuffers[frameIndex],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores =
            (VkSemaphore[]){
                app->renderer.renderFinishedSemaphores[imageIndex] },
    };

    ASSERT(vkQueueSubmit(app->context.queue, 1, &submitInfo,
                         app->renderer.inFlightFences[frameIndex])
               == VK_SUCCESS,
           "Failed to submit draw command buffer for frame %u", frameIndex);
}