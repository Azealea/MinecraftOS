#include "vertex.h"

#include <string.h>

#include "app.h"
#include "block.h"
#include "chunk.h"
#include "vk/buffer/buffer.h"

#define VERT_MAX 5000
#define INDICES_MAX VERT_MAX * 3

Vertex vertices[VERT_MAX] = {};
size_t vertices_count = 0;

uint16_t indices[VERT_MAX] = {};
size_t indices_count = 0;

void recreate_vertices(const Chunk* chunk)
{
    vertices_count = 0;
    indices_count = 0;

    static const int neighbourDirs[6][3] = {
        {1, 0, 0}, // +X
        {-1, 0, 0}, // -X
        {0, 1, 0}, // +Y
        {0, -1, 0}, // -Y
        {0, 0, 1}, // +Z
        {0, 0, -1}, // -Z
    };

    static const int faceVertsOffsets[6][4][3] = {
        {{1, 0, 0}, {1, 1, 0}, {1, 1, 1}, {1, 0, 1}}, // +X
        {{0, 0, 1}, {0, 1, 1}, {0, 1, 0}, {0, 0, 0}}, // -X
        {{0, 1, 1}, {1, 1, 1}, {1, 1, 0}, {0, 1, 0}}, // +Y
        {{0, 0, 0}, {1, 0, 0}, {1, 0, 1}, {0, 0, 1}}, // -Y
        {{1, 0, 1}, {1, 1, 1}, {0, 1, 1}, {0, 0, 1}}, // +Z
        {{0, 0, 0}, {0, 1, 0}, {1, 1, 0}, {1, 0, 0}}, // -Z
    };

    static const float uvs[4][2] = {
        {0.f, 0.f},
        {1.f, 0.f},
        {1.f, 1.f},
        {0.f, 1.f},
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

                    uint16_t baseIndex = (uint16_t)vertices_count;

                    for (int v = 0; v < 4; v++)
                    {
                        vertices[vertices_count++] = (Vertex){
                            .pos =
                                {
                                    chunk->x + x + faceVertsOffsets[f][v][0],
                                    chunk->y + y + faceVertsOffsets[f][v][1],
                                    chunk->z + z + faceVertsOffsets[f][v][2],
                                },
                            .texCoord =
                                {
                                    uvs[v][0],
                                    uvs[v][1],
                                },
                            .textureid = BlockTexture[b->type][f],
                        };
                    }

                    indices[indices_count++] = baseIndex + 0;
                    indices[indices_count++] = baseIndex + 1;
                    indices[indices_count++] = baseIndex + 2;

                    indices[indices_count++] = baseIndex + 2;
                    indices[indices_count++] = baseIndex + 3;
                    indices[indices_count++] = baseIndex + 0;
                }
            }
}

uint32_t vertex_count(void)
{
    return vertices_count;
}

uint32_t index_count(void)
{
    return indices_count;
}

VkVertexInputBindingDescription get_binding_description()
{
    VkVertexInputBindingDescription bindingDescription = {
        .binding = 0,
        .stride = sizeof(Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
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
                .offset = offsetof(Vertex, pos),
            },
        [1] =
            {
                .binding = 0,
                .location = 1,
                .format = VK_FORMAT_R32G32_SFLOAT,
                .offset = offsetof(Vertex, texCoord),
            },
        [2] =
            {
                .binding = 0,
                .location = 2,
                .format = VK_FORMAT_R32_SINT,
                .offset = offsetof(Vertex, textureid),
            },
    };

    *out_size = COUNTOF(attributeDescriptions);
    return attributeDescriptions;
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
