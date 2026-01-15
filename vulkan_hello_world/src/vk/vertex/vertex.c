#include "vertex.h"

#include <string.h>

#include "utils.h"
#include "vk/vk_device.h"
#include "vulkan/vulkan_core.h"

const Vertex vertices[] = {
    { { 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f } },
    { { 0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
    { { -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },
};

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

void create_vertex_buffer(App* app)
{
    VkDeviceSize bufferSize = sizeof(vertices);

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    create_buffer(app, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                      | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                  &stagingBuffer, &stagingBufferMemory);

    void* data;
    ASSERTVK(vkMapMemory(app->context.device, stagingBufferMemory, 0,
                         bufferSize, 0, &data),
             "Failed to map");
    memcpy(data, vertices, (size_t)bufferSize);
    vkUnmapMemory(app->context.device, stagingBufferMemory);

    create_buffer(app, bufferSize,
                  VK_BUFFER_USAGE_TRANSFER_DST_BIT
                      | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &app->vertexBuffer,
                  &app->vertexBufferMemory);

    copyBuffer(app, stagingBuffer, app->vertexBuffer, bufferSize);

    vkDestroyBuffer(app->context.device, stagingBuffer, nullptr);
    vkFreeMemory(app->context.device, stagingBufferMemory, nullptr);
}

void destroy_vertex_buffer(App* app)
{
    vkDestroyBuffer(app->context.device, app->vertexBuffer, nullptr);
    vkFreeMemory(app->context.device, app->vertexBufferMemory, nullptr);
}

uint32_t vertex_count(void)
{
    return 3;
}
