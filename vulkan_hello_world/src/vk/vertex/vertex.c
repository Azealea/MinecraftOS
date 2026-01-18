#include "vertex.h"

#include <string.h>
#include <vulkan/vulkan_core.h>

#include "utils.h"
#include "vk/vertex/buffer.h"

const Vertex vertices[] = { { { -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
                            { { 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
                            { { 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },
                            { { -0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f } } };

const uint16_t indices[] = { 0, 2, 1, 2, 0, 3 };

uint32_t vertex_count(void)
{
    return sizeof(vertices) / sizeof(vertices[0]);
}

uint32_t index_count(void)
{
    return sizeof(indices) / sizeof(indices[0]);
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

    vkDestroyBuffer(app->context.device, stagingBuffer, app->allocator);
    vkFreeMemory(app->context.device, stagingBufferMemory, app->allocator);
}

void destroy_vertex_buffer(App* app)
{
    vkDestroyBuffer(app->context.device, app->vertexBuffer, app->allocator);
    vkFreeMemory(app->context.device, app->vertexBufferMemory, app->allocator);
}

void create_index_buffer(App* app)
{
    VkDeviceSize bufferSize = sizeof(indices[0]) * index_count();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    create_buffer(app, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                      | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                  &stagingBuffer, &stagingBufferMemory);

    void* data;
    vkMapMemory(app->context.device, stagingBufferMemory, 0, bufferSize, 0,
                &data);
    memcpy(data, indices, (size_t)bufferSize);
    vkUnmapMemory(app->context.device, stagingBufferMemory);

    create_buffer(app, bufferSize,
                  VK_BUFFER_USAGE_TRANSFER_DST_BIT
                      | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &app->indexBuffer,
                  &app->indexBufferMemory);

    copyBuffer(app, stagingBuffer, app->indexBuffer, bufferSize);

    vkDestroyBuffer(app->context.device, stagingBuffer, app->allocator);
    vkFreeMemory(app->context.device, stagingBufferMemory, app->allocator);
}

void destroy_index_buffer(App* app)
{
    vkDestroyBuffer(app->context.device, app->indexBuffer, app->allocator);
    vkFreeMemory(app->context.device, app->indexBufferMemory, app->allocator);
}
