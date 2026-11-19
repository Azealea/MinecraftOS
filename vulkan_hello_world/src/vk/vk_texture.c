#include "vk_texture.h"

#include <string.h>

#include "utils/utils.h"
#include "vk/gpu_resources.h"
#include "vk/renderer/vk_command.h"
#include "voxel/textures/builder/array_atlas.h"
#include "vulkan/vulkan_core.h"

static void transition_image_layout(VkCommandBuffer cmd, VkImage image,
                                    uint32_t layerCount, VkImageLayout oldLayout,
                                    VkImageLayout newLayout)
{
    VkImageMemoryBarrier2 barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
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

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED
        && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
        barrier.srcAccessMask = VK_ACCESS_2_NONE;
        barrier.dstStageMask = VK_PIPELINE_STAGE_2_COPY_BIT;
        barrier.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
             && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT;
        barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
        barrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
        barrier.dstAccessMask = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;
    }
    else
    {
        ASSERT(false, "Unsupported layout transition!");
    }

    vkCmdPipelineBarrier2(cmd,
                          &(VkDependencyInfo){
                              .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                              .imageMemoryBarrierCount = 1,
                              .pImageMemoryBarriers = &barrier,
                          });
}

void create_texture_image_from_atlas(App* app, VECTOR(texture_t)* atlas)
{
    uint32_t layerCount = atlas->size;
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
        memcpy((char*)data + layerSize * i, atlas->data[i], layerSize);
    }

    vkUnmapMemory(app->renderer.context.device, staging.mem);

    gpu_image_create(app, TEXTURE_WIDTH_HEIGHT, TEXTURE_WIDTH_HEIGHT, layerCount,
                     VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
                     VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT,
                     &app->renderer.texture.image);

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

    VkCommandBuffer cmd = begin_single_time_commands(app);

    transition_image_layout(cmd, app->renderer.texture.image.img, layerCount,
                            VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    vkCmdCopyBufferToImage(cmd, staging.buf, app->renderer.texture.image.img,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, layerCount, regions);

    transition_image_layout(cmd, app->renderer.texture.image.img, layerCount,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    end_single_time_commands(app, cmd);

    free(regions);
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

void destroy_texture_stuff(App* app)
{
    destroy_texture_sampler(app);
    gpu_image_destroy(app, &app->renderer.texture.image);
}
