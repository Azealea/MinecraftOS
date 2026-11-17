#include "vk/renderer/vk_command.h"

#include <stdlib.h>

VkCommandBuffer begin_single_time_commands(App* app)
{
    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandPool = app->renderer.sync.pool,
        .commandBufferCount = 1,
    };

    VkCommandBuffer commandBuffer;
    ASSERTVK(vkAllocateCommandBuffers(app->renderer.context.device, &allocInfo,
                                      &commandBuffer),
             "Failed to allocate single-time command buffer");

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    ASSERTVK(vkBeginCommandBuffer(commandBuffer, &beginInfo),
             "Failed to begin single-time command buffer");

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

    vkQueueSubmit(app->renderer.context.queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(app->renderer.context.queue);

    vkFreeCommandBuffers(app->renderer.context.device, app->renderer.sync.pool, 1,
                         &commandBuffer);
}

void create_command_pool(App* app)
{
    ASSERTVK(
        vkCreateCommandPool(app->renderer.context.device,
                            &(VkCommandPoolCreateInfo){
                                .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                                .queueFamilyIndex = app->renderer.context.queueFamily,
                                .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                            },
                            app->renderer.allocator, &app->renderer.sync.pool),
        "Couldn't create command pool")
}

void destroy_command_pool(App* app)
{
    vkDestroyCommandPool(app->renderer.context.device, app->renderer.sync.pool,
                         app->renderer.allocator);
}

void allocate_command_buffer(App* app)
{
    app->renderer.sync.buffers = malloc(sizeof(VkCommandBuffer) * app->maxFramesInFlight);
    ASSERTVK(vkAllocateCommandBuffers(
                 app->renderer.context.device,
                 &(VkCommandBufferAllocateInfo){
                     .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                     .commandPool = app->renderer.sync.pool,
                     .commandBufferCount = app->maxFramesInFlight,
                     .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                 },
                 app->renderer.sync.buffers),
             "Couldn't allocate command buffer");
}

void destroy_command_buffer(App* app)
{
    free(app->renderer.sync.buffers);
}

void record_command_buffer(App* app, uint32_t imageIndex, uint32_t frameIndex)
{
    VkCommandBuffer cmd = app->renderer.sync.buffers[frameIndex];
    ASSERTVK(vkResetCommandBuffer(cmd, 0), "Failed to reset command buffer");

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    };
    ASSERTVK(vkBeginCommandBuffer(cmd, &beginInfo),
             "Failed to begin recording command buffer");

    VkImage swapImage = app->renderer.swapchain.images[imageIndex];

    vkCmdPipelineBarrier2(
        cmd,
        &(VkDependencyInfo){
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 2,
            .pImageMemoryBarriers =
                (VkImageMemoryBarrier2[]){
                    // color
                    {
                        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                        .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .srcAccessMask = VK_ACCESS_2_NONE,
                        .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                        .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                        .image = swapImage,
                        .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
                    },
                    // depth
                    {
                        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                        .srcStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT
                            | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
                        .srcAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                        .dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT
                            | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
                        .dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT
                            | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                        .newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                        .image = app->renderer.depth.img,
                        .subresourceRange = {VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1},
                    },
                },
        });

    vkCmdBeginRendering(
        cmd,
        &(VkRenderingInfo){
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .renderArea = {{0, 0}, app->renderer.swapchain.extent},
            .layerCount = 1,
            .colorAttachmentCount = 1,
            .pColorAttachments =
                &(VkRenderingAttachmentInfo){
                    .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
                    .imageView = app->renderer.swapchain.imageViews[imageIndex],
                    .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                    .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                    .clearValue = app->backgroundColor,
                },
            .pDepthAttachment =
                &(VkRenderingAttachmentInfo){
                    .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
                    .imageView = app->renderer.depth.view,
                    .imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                    .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                    .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                    .clearValue.depthStencil = {1.0f, 0},
                },
        });

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      app->renderer.pipeline.graphics);

    VkBuffer instanceBuffers[] = {app->renderer.buffers.vertex.buf};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(cmd, 0, 1, instanceBuffers, offsets);

    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            app->renderer.pipeline.layout, 0, 1,
                            &app->renderer.descriptors.sets[frameIndex], 0, NULL);

    MAP_FOR_EACH(app->world.chunks, entry)
    {
        Chunk* c = entry->value;
        if (c->bucket_ptr == NULL || c->face_count == 0)
            continue;
        vkCmdDraw(cmd, 6, c->face_count, 0,
                  bucket_alloc_index_of(&app->mesher.mesh, c->bucket_ptr));
    }

    vkCmdEndRendering(cmd);

    vkCmdPipelineBarrier2(
        cmd,
        &(VkDependencyInfo){
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers =
                &(VkImageMemoryBarrier2){
                    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                    .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                    .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                    .dstStageMask = VK_PIPELINE_STAGE_2_NONE,
                    .dstAccessMask = VK_ACCESS_2_NONE,
                    .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                    .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                    .image = swapImage,
                    .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
                },
        });

    ASSERTVK(vkEndCommandBuffer(cmd), "Failed to record command buffer for image %u",
             imageIndex);
}

void submit_command_buffer(App* app, uint32_t imageIndex, uint32_t frameIndex)
{
    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = (VkSemaphore[]){app->renderer.sync.imageAvailable[frameIndex]},
        .pWaitDstStageMask =
            (VkPipelineStageFlags[]){
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            },
        .commandBufferCount = 1,
        .pCommandBuffers = &app->renderer.sync.buffers[frameIndex],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores =
            (VkSemaphore[]){app->renderer.sync.renderFinished[imageIndex]},
    };

    ASSERTVK(vkQueueSubmit(app->renderer.context.queue, 1, &submitInfo,
                           app->renderer.sync.inFlight[frameIndex]),
             "Failed to submit draw command buffer for frame %u", frameIndex);
}
