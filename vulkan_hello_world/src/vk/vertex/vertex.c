#include "vertex.h"

#include <string.h>

#include "vk/vk_device.h"

const Vertex vertices[] = {
    { { 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f } },
    { { 0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
    { { -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },
};

void create_vertex_buffer(App* app)
{
    VkDeviceSize bufferSize = sizeof(vertices);

    VkBufferCreateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = bufferSize,
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    ASSERT(vkCreateBuffer(app->context.device, &bufferInfo, app->allocator,
                          &app->vertexBuffer)
               == VK_SUCCESS,
           "Failed to create vertex buffer");

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(app->context.device, app->vertexBuffer,
                                  &memRequirements);

    VkMemoryAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex =
            find_memory_type(app, memRequirements.memoryTypeBits,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                                 | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
    };

    ASSERT(vkAllocateMemory(app->context.device, &allocInfo, app->allocator,
                            &app->vertexBufferMemory)
               == VK_SUCCESS,
           "Failed to allocate vertex buffer memory");

    vkBindBufferMemory(app->context.device, app->vertexBuffer,
                       app->vertexBufferMemory, 0);

    void* data;
    vkMapMemory(app->context.device, app->vertexBufferMemory, 0, bufferSize, 0,
                &data);
    memcpy(data, vertices, (size_t)bufferSize);
    vkUnmapMemory(app->context.device, app->vertexBufferMemory);
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