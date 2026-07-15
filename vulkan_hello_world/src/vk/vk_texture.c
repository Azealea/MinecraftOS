#include "vk_texture.h"

#include <string.h>

#include "utils/utils.h"
#include "vk/gpu_resources.h"
#include "vk/renderer/vk_command.h"
#include "voxel/textures/array_atlas.h"
#include "voxel/textures/block_face_textures.h"

void transition_image_layout(App* app, VkImage image, uint32_t layerCount,
                             VkImageLayout oldLayout, VkImageLayout newLayout)
{
    VkCommandBuffer commandBuffer = begin_single_time_commands(app);

    VkImageMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
        .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .subresourceRange.baseMipLevel = 0,
        .subresourceRange.levelCount = 1,
        .subresourceRange.baseArrayLayer = 0,
        .subresourceRange.layerCount = layerCount,
    };

    VkPipelineStageFlags sourceStage = {};
    VkPipelineStageFlags destinationStage = {};

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED
        && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
             && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        ASSERT(false, "Unsupported layout transition!");
    }

    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0,
                         nullptr, 1, &barrier);

    end_single_time_commands(app, commandBuffer);
}

void create_texture_image_from_atlas(App* app, ArrayAtlas* atlas)
{
    uint32_t layerCount = atlas->count;
    VkDeviceSize layerSize = TEXTURE_WIDTH_HEIGHT * TEXTURE_WIDTH_HEIGHT * 4;
    VkDeviceSize totalSize = layerSize * layerCount;

    GpuBuffer staging;
    gpu_buffer_create(app, totalSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                          | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      &staging);

    void* data;
    ASSERTVK(
        vkMapMemory(app->renderer.context.device, staging.mem, 0, totalSize, 0, &data),
        "Failed to map staging buffer");

    for (uint32_t i = 0; i < layerCount; i++)
    {
        memcpy((char*)data + layerSize * i, atlas->pixels[i], layerSize);
    }

    vkUnmapMemory(app->renderer.context.device, staging.mem);

    gpu_image_create(app, TEXTURE_WIDTH_HEIGHT, TEXTURE_WIDTH_HEIGHT, layerCount,
                     VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
                     VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT,
                     &app->renderer.texture.image);

    transition_image_layout(app, app->renderer.texture.image.img, layerCount,
                            VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VkCommandBuffer commandBuffer = begin_single_time_commands(app);
    VkBufferImageCopy* regions = malloc(sizeof(VkBufferImageCopy) * layerCount);

    for (uint32_t i = 0; i < layerCount; i++)
    {
        regions[i] = (VkBufferImageCopy){
            .bufferOffset = layerSize * i,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,
            .imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .imageSubresource.mipLevel = 0,
            .imageSubresource.baseArrayLayer = i,
            .imageSubresource.layerCount = 1,
            .imageOffset = (VkOffset3D){0, 0, 0},
            .imageExtent = (VkExtent3D){TEXTURE_WIDTH_HEIGHT, TEXTURE_WIDTH_HEIGHT, 1},
        };
    }

    vkCmdCopyBufferToImage(commandBuffer, staging.buf, app->renderer.texture.image.img,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, layerCount, regions);

    end_single_time_commands(app, commandBuffer);
    free(regions);

    transition_image_layout(app, app->renderer.texture.image.img, layerCount,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    gpu_buffer_destroy(app, &staging);
}

void create_texture_sampler(App* app)
{
    VkSamplerCreateInfo samplerInfo = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = VK_FILTER_NEAREST,
        .minFilter = VK_FILTER_NEAREST,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .anisotropyEnable = VK_FALSE,
        .maxAnisotropy = 1.0f,
        .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
        .unnormalizedCoordinates = VK_FALSE,
        .compareEnable = VK_FALSE,
        .compareOp = VK_COMPARE_OP_ALWAYS,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
        .minLod = 0.0f,
        .maxLod = 0.0f,
    };

    ASSERTVK(vkCreateSampler(app->renderer.context.device, &samplerInfo,
                             app->renderer.allocator, &app->renderer.texture.sampler),
             "failed to create texture sampler!");
}

void destroy_texture_sampler(App* app)
{
    vkDestroySampler(app->renderer.context.device, app->renderer.texture.sampler,
                     app->renderer.allocator);
}

void create_texture_stuff(App* app)
{
    ArrayAtlas atlas = {0};
    atlas_init(&atlas);
    load_texture_into_atlas(&atlas, app->block_faces);
    //    debug_print_block_faces(app->block_faces);
    create_texture_image_from_atlas(app, &atlas);
    create_texture_sampler(app);
    atlas_free(&atlas);
}
void destroy_texture_stuff(App* app)
{
    destroy_texture_sampler(app);
    gpu_image_destroy(app, &app->renderer.texture.image);
}
