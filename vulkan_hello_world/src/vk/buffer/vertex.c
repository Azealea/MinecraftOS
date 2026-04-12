#include "vertex.h"

#include <string.h>

#include "app.h"
#include "vk/buffer/buffer.h"
#include "voxel/chunk.h"
#include "voxel/textures/face_texture.h"

#define INSTANCE_MAX (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 6)

Face face_instances[INSTANCE_MAX];
size_t face_instance_count = 0;

void recreate_vertices(const Chunk* chunk)
{
    face_instance_count = 0;

    static const int neighbourDirs[6][3] = {
        {1, 0, 0}, // +X
        {-1, 0, 0}, // -X
        {0, 1, 0}, // +Y
        {0, -1, 0}, // -Y
        {0, 0, 1}, // +Z
        {0, 0, -1}, // -Z
    };

    for (size_t x = 0; x < CHUNK_SIZE; x++)
        for (size_t y = 0; y < CHUNK_SIZE; y++)
            for (size_t z = 0; z < CHUNK_SIZE; z++)
            {
                const Block* b = chunk_get(chunk, x, y, z);
                if (b->type == BLK_AIR)
                    continue;

                for (int f = 0; f < 6; f++)
                {
                    int nx = x + neighbourDirs[f][0];
                    int ny = y + neighbourDirs[f][1];
                    int nz = z + neighbourDirs[f][2];

                    if (chunk_is_in_bound(nx, ny, nz)
                        && chunk_get(chunk, nx, ny, nz)->type != BLK_AIR)
                        continue;

                    face_instances[face_instance_count++] = (Face){
                        .pos = {chunk->x + x, chunk->y + y, chunk->z + z},
                        .face_id = f,

                        .texture_id = face_texture_resolve(
                            &BlockFaces[b->type][f], 0, 0, 0),
                    };
                }
            }
}

uint32_t instance_count(void)
{
    return (uint32_t)face_instance_count;
}

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
    VkDeviceSize bufferSize = sizeof(face_instances);

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
    memcpy(data, face_instances, (size_t)bufferSize);
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
