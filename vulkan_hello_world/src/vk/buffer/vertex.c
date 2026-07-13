#include "vertex.h"

#include <string.h>

#include "app.h"
#include "vk/buffer/buffer.h"
#include "vk/gpu_resources.h"
#include "voxel/bucket_alloc.h"
#include "voxel/mesh.h"
#include "voxel/world.h"

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

void create_vertex_buffer(App* app)
{
    VkDeviceSize bufferSize = (VkDeviceSize)INSTANCE_MAX * sizeof(Face);
    gpu_buffer_create(app, bufferSize,
                      VK_BUFFER_USAGE_TRANSFER_DST_BIT
                          | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &app->renderer.buffers.vertex);
}

void update_vertex_buffer(App* app, uint32_t bucket_index, uint32_t face_count)
{
    vkDeviceWaitIdle(app->renderer.context.device);
    const Face* faces = bucket_ptr(&app->world.mesh, bucket_index);
    VkDeviceSize dataSize = (VkDeviceSize)face_count * sizeof(Face);
    VkDeviceSize dstOffset = (VkDeviceSize)bucket_index * BUCKET_FACE_CAP * sizeof(Face);

    GpuBuffer staging;
    gpu_buffer_create(app, dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                          | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      &staging);

    void* data;
    ASSERTVK(
        vkMapMemory(app->renderer.context.device, staging.mem, 0, dataSize, 0, &data),
        "Failed to map staging buffer");
    memcpy(data, faces, dataSize);
    vkUnmapMemory(app->renderer.context.device, staging.mem);

    copyBufferAt(app, staging.buf, app->renderer.buffers.vertex.buf, dataSize, dstOffset);
    gpu_buffer_destroy(app, &staging);
}

void destroy_vertex_buffer(App* app)
{
    gpu_buffer_destroy(app, &app->renderer.buffers.vertex);
}

void vk_rebuild_mesh(App* app, ChunkPos pos)
{
    Chunk* c = world_get_chunk(&app->world, pos);
    ASSERT(c != NULL, "vk_rebuild_mesh: chunk " VEC3_FMT " does not exist",
           VEC3_ARGS(pos));
    if (c->bucket_index == NO_BUCKET)
        c->bucket_index = bucket_alloc_acquire(&app->world.mesh);
    c->face_count =
        generate_chunk_mesh(c, pos, bucket_ptr(&app->world.mesh, c->bucket_index),
                            (const FaceTexture(*)[FACE_COUNT])app->block_faces);
    update_vertex_buffer(app, c->bucket_index, c->face_count);
}
