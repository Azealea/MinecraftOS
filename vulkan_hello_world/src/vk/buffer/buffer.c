#include "buffer.h"

#include "vk/vk_device.h"

void create_buffer(App* app, VkDeviceSize size, VkBufferUsageFlags usage,
                   VkMemoryPropertyFlags properties, VkBuffer* buffer,
                   VkDeviceMemory* bufferMemory)
{
    VkBufferCreateInfo bufferInfo = { .sType =
                                          VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                                      .size = size,
                                      .usage = usage,
                                      .sharingMode =
                                          VK_SHARING_MODE_EXCLUSIVE };

    ASSERTVK(vkCreateBuffer(app->context.device, &bufferInfo, app->allocator,
                            buffer),
             "Failed to create vertex buffer");

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(app->context.device, *buffer,
                                  &memRequirements);

    VkMemoryAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex =
            find_memory_type(app, memRequirements.memoryTypeBits, properties)
    };

    ASSERTVK(vkAllocateMemory(app->context.device, &allocInfo, app->allocator,
                              bufferMemory),
             "Failed to allocate vertex buffer memory");

    vkBindBufferMemory(app->context.device, *buffer, *bufferMemory, 0);
}

void copyBuffer(App* app, VkBuffer srcBuffer, VkBuffer dstBuffer,
                VkDeviceSize size)
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

    VkBufferCopy copyRegion = { .size = size };
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

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
