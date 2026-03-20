#include "vk/renderer/vk_command.h"

#include <stdlib.h>

#include "vk/buffer/vertex.h"

VkCommandBuffer begin_single_time_commands(App* app)
{
    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandPool = app->renderer.commandPool,
        .commandBufferCount = 1,
    };

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(app->context.device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void end_single_time_commands(App* app, VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer,
    };

    vkQueueSubmit(app->context.queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(app->context.queue);

    vkFreeCommandBuffers(app->context.device, app->renderer.commandPool, 1,
                         &commandBuffer);
}

void create_command_pool(App* app)
{
    ASSERTVK(vkCreateCommandPool(
                 app->context.device,
                 &(VkCommandPoolCreateInfo){
                     .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                     .queueFamilyIndex = app->context.queueFamily,
                     .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                 },
                 app->allocator, &app->renderer.commandPool),
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
    ASSERTVK(vkAllocateCommandBuffers(
                 app->context.device,
                 &(VkCommandBufferAllocateInfo){
                     .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                     .commandPool = app->renderer.commandPool,
                     .commandBufferCount = app->maxFramesInFlight,
                     .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                 },
                 app->renderer.commandBuffers),
             "Couldn't allocate command buffer");
}

void destroy_command_buffer(App* app)
{
    free(app->renderer.commandBuffers);
}

void record_command_buffer(App* app, uint32_t imageIndex, uint32_t frameIndex)
{
    VkCommandBuffer cmd = app->renderer.commandBuffers[frameIndex];

    ASSERTVK(vkResetCommandBuffer(cmd, 0), "Failed to reset command buffer");

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = 0,
        .pInheritanceInfo = NULL,
    };

    ASSERTVK(vkBeginCommandBuffer(cmd, &beginInfo),
             "Failed to begin recording command buffer");

    VkClearValue clearValues[2];
    clearValues[0] = app->backgroundColor;
    clearValues[1].depthStencil = (VkClearDepthStencilValue){1.0f, 0};

    VkRenderPassBeginInfo renderPassInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = NULL,
        .renderPass = app->renderer.renderpass,
        .framebuffer = app->renderer.framebuffers[imageIndex],
        .renderArea =
            {
                .offset = {0, 0},
                .extent = app->swapchain.imageExtent,
            },
        .clearValueCount = COUNTOF(clearValues),
        .pClearValues = clearValues,
    };

    vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      app->renderer.graphicsPipeline);

    VkBuffer vertexBuffers[] = {app->vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(cmd, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(cmd, app->indexBuffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            app->renderer.pipelineLayout, 0, 1,
                            &app->descriptorSets[frameIndex], 0, nullptr);

    vkCmdDrawIndexed(cmd, index_count(), 1, 0, 0, 0);

    vkCmdEndRenderPass(cmd);

    ASSERTVK(vkEndCommandBuffer(cmd),
             "Failed to record command buffer for image %u", imageIndex);
}

void submit_command_buffer(App* app, uint32_t imageIndex, uint32_t frameIndex)
{
    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores =
            (VkSemaphore[]){app->renderer.imageAvailableSemaphores[frameIndex]},
        .pWaitDstStageMask =
            (VkPipelineStageFlags[]){
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            },
        .commandBufferCount = 1,
        .pCommandBuffers = &app->renderer.commandBuffers[frameIndex],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores =
            (VkSemaphore[]){app->renderer.renderFinishedSemaphores[imageIndex]},
    };

    ASSERTVK(vkQueueSubmit(app->context.queue, 1, &submitInfo,
                           app->renderer.inFlightFences[frameIndex]),
             "Failed to submit draw command buffer for frame %u", frameIndex);
}
