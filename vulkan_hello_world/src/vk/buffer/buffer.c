#include "buffer.h"

#include "vk/renderer/vk_command.h"
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
    VkCommandBuffer commandBuffer = begin_single_time_commands(app);

    VkBufferCopy copyRegion = {
        .size = size,
    };

    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    end_single_time_commands(app, commandBuffer);
}
