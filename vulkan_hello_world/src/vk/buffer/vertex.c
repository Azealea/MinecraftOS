#include "vertex.h"

#include <string.h>

#include "app.h"
#include "vk/buffer/buffer.h"
#include "vk/gpu_resources.h"

VkVertexInputBindingDescription get_binding_description()
{
    VkVertexInputBindingDescription bindingDescription = {
        .binding = 0,
        .stride = sizeof(Face),
        .inputRate = VK_VERTEX_INPUT_RATE_INSTANCE,
    };

    return bindingDescription;
}

VkVertexInputAttributeDescription* get_attribute_descriptions(int* out_size)
{
    static VkVertexInputAttributeDescription attributeDescriptions[] = {
        [0] =
            {
                .binding = 0,
                .location = 0,
                .format = VK_FORMAT_R32G32B32_SINT,
                .offset = offsetof(Face, pos),
            },
        [1] =
            {
                .binding = 0,
                .location = 1,
                .format = VK_FORMAT_R32_SINT,
                .offset = offsetof(Face, face_id),
            },
        [2] =
            {
                .binding = 0,
                .location = 2,
                .format = VK_FORMAT_R32_SINT,
                .offset = offsetof(Face, texture_id),
            },
    };

    *out_size = COUNTOF(attributeDescriptions);
    return attributeDescriptions;
}

void create_vertex_buffer(App* app, const Face* faces, uint32_t count)
{
    VkDeviceSize bufferSize = INSTANCE_MAX * sizeof(Face);
    VkDeviceSize dataSize = count * sizeof(Face);

    GpuBuffer staging;
    gpu_buffer_create(app, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                          | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      &staging);

    void* data;
    ASSERTVK(vkMapMemory(app->renderer.context.device, staging.mem, 0,
                         bufferSize, 0, &data),
             "Failed to map");
    memcpy(data, faces, (size_t)dataSize);
    vkUnmapMemory(app->renderer.context.device, staging.mem);

    gpu_buffer_create(
        app, bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &app->renderer.buffers.vertex);

    copyBuffer(app, staging.buf, app->renderer.buffers.vertex.buf, bufferSize);
    gpu_buffer_destroy(app, &staging);
}

void destroy_vertex_buffer(App* app)
{
    gpu_buffer_destroy(app, &app->renderer.buffers.vertex);
}
