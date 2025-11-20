#include "vk/renderer/vk_command.h"

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
    ASSERT(vkAllocateCommandBuffers(
               app->context.device,
               &(VkCommandBufferAllocateInfo){
                   .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                   .commandPool = app->renderer.commandPool,
                   .commandBufferCount = 1,
                   .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
               },
               &app->renderer.commandBuffer)
               == VK_SUCCESS,
           "Couldn't allocate command buffer")
}

void record_command_buffer(App* app)
{
    VkCommandBuffer commandBuffer = app->renderer.commandBuffer;
    uint32_t imageAcquiredIndex = app->swapchain.imageAcquiredIndex;

    ASSERT(vkBeginCommandBuffer(
               commandBuffer,
               &(VkCommandBufferBeginInfo){
                   .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
               })
               == VK_SUCCESS,
           "Couldn't begin command buffer for frame")

    vkCmdBeginRenderPass(
        app->renderer.commandBuffer,
        &(VkRenderPassBeginInfo){
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = app->renderer.renderpass,
            .framebuffer = app->renderer.framebuffers[imageAcquiredIndex],
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

void submit_command_buffer(App* app)
{
    ASSERT(vkQueueSubmit(
               app->context.queue, 1,
               &(VkSubmitInfo){
                   .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                   .commandBufferCount = 1,
                   .pCommandBuffers = &app->renderer.commandBuffer,
                   .waitSemaphoreCount = 1,
                   .pWaitSemaphores = &app->renderer.imageAcquiredSemaphore,
                   .signalSemaphoreCount = 1,
                   .pSignalSemaphores = &app->renderer.renderFinishedSemaphore,
                   .pWaitDstStageMask =
                       (VkPipelineStageFlags[]){
                           VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                       } },
               app->renderer.inFlightFence)
               == VK_SUCCESS,
           "Couldn't submit command buffer");
}