#include "vk_texture.h"

#include <string.h>

#include "utils/utils.h"
#include "vk/buffer/buffer.h"
#include "vk/renderer/vk_command.h"
#include "vk/vk_device.h"
#include "voxel/assets.h"
#include "voxel/textures/array_atlas.h"

void create_image(App* app, uint32_t width, uint32_t height, VkFormat format,
                  VkImageTiling tiling, VkImageUsageFlags usage,
                  VkMemoryPropertyFlags properties, VkImage* image,
                  VkDeviceMemory* imageMemory)
{
    VkImageCreateInfo imageInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .extent.width = width,
        .extent.height = height,
        .extent.depth = 1,
        .mipLevels = 1,
        .arrayLayers = 1,
        .format = format,
        .tiling = tiling,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .usage = usage,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    ASSERTVK(
        vkCreateImage(app->context.device, &imageInfo, app->allocator, image),
        "failed to create image!")

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(app->context.device, *image, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex =
            find_memory_type(app, memRequirements.memoryTypeBits, properties),
    };

    ASSERTVK(vkAllocateMemory(app->context.device, &allocInfo, app->allocator,
                              imageMemory),
             "failed to allocate image memory!");

    vkBindImageMemory(app->context.device, *image, *imageMemory, 0);
}

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

    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0,
                         nullptr, 0, nullptr, 1, &barrier);

    end_single_time_commands(app, commandBuffer);
}

void copyBufferToImage(App* app, VkBuffer buffer, VkImage image, uint32_t width,
                       uint32_t height)
{
    VkCommandBuffer commandBuffer = begin_single_time_commands(app);

    VkBufferImageCopy region = {
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .imageSubresource.mipLevel = 0,
        .imageSubresource.baseArrayLayer = 0,
        .imageSubresource.layerCount = 1,
        .imageOffset = {0, 0, 0},
        .imageExtent = {width, height, 1},
    };

    vkCmdCopyBufferToImage(commandBuffer, buffer, image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    end_single_time_commands(app, commandBuffer);
}

void create_texture_image_from_atlas(App* app, ArrayAtlas* atlas)
{
    assert(atlas != NULL);
    assert(atlas->count > 0);
    uint32_t layerCount = atlas->count;
    VkDeviceSize layerSize = TEXTURE_WIDTH_HEIGHT * TEXTURE_WIDTH_HEIGHT * 4;
    VkDeviceSize totalSize = layerSize * layerCount;
    assert(totalSize > 0);

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    create_buffer(app, totalSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                      | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                  &stagingBuffer, &stagingBufferMemory);

    void* data;
    vkMapMemory(app->context.device, stagingBufferMemory, 0, totalSize, 0,
                &data);

    for (uint32_t i = 0; i < layerCount; i++)
    {
        memcpy((char*)data + layerSize * i, atlas->pixels[i], layerSize);
    }

    vkUnmapMemory(app->context.device, stagingBufferMemory);

    VkImageCreateInfo imageInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .extent.width = TEXTURE_WIDTH_HEIGHT,
        .extent.height = TEXTURE_WIDTH_HEIGHT,
        .extent.depth = 1,
        .mipLevels = 1,
        .arrayLayers = layerCount,
        .format = VK_FORMAT_R8G8B8A8_SRGB,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    ASSERTVK(vkCreateImage(app->context.device, &imageInfo, app->allocator,
                           &app->textureImage),
             "failed to create image!")

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(app->context.device, app->textureImage,
                                 &memRequirements);

    VkMemoryAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex =
            find_memory_type(app, memRequirements.memoryTypeBits,
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
    };

    ASSERTVK(vkAllocateMemory(app->context.device, &allocInfo, app->allocator,
                              &app->textureImageMemory),
             "failed to allocate image memory!");

    vkBindImageMemory(app->context.device, app->textureImage,
                      app->textureImageMemory, 0);

    transition_image_layout(app, app->textureImage, layerCount,
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
            .imageExtent =
                (VkExtent3D){TEXTURE_WIDTH_HEIGHT, TEXTURE_WIDTH_HEIGHT, 1},
        };
    }

    vkCmdCopyBufferToImage(commandBuffer, stagingBuffer, app->textureImage,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, layerCount,
                           regions);

    end_single_time_commands(app, commandBuffer);
    free(regions);

    transition_image_layout(app, app->textureImage, layerCount,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(app->context.device, stagingBuffer, app->allocator);
    vkFreeMemory(app->context.device, stagingBufferMemory, app->allocator);
}

void destroy_texture_image(App* app)
{
    vkDestroyImage(app->context.device, app->textureImage, app->allocator);
    vkFreeMemory(app->context.device, app->textureImageMemory, app->allocator);
}

void create_texture_image_view(App* app, uint32_t layerCount)
{
    VkImageViewCreateInfo viewInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = app->textureImage,
        .viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY,
        .format = VK_FORMAT_R8G8B8A8_SRGB,
        .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .subresourceRange.baseMipLevel = 0,
        .subresourceRange.levelCount = 1,
        .subresourceRange.baseArrayLayer = 0,
        .subresourceRange.layerCount = layerCount,
    };

    ASSERTVK(vkCreateImageView(app->context.device, &viewInfo, app->allocator,
                               &app->textureImageView),
             "failed to create image view!")
}

void destroy_texture_image_view(App* app)
{
    vkDestroyImageView(app->context.device, app->textureImageView,
                       app->allocator);
}

void create_texture_sampler(App* app)
{
    VkPhysicalDeviceProperties properties = {};
    vkGetPhysicalDeviceProperties(app->context.physicalDevice, &properties);

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

    ASSERTVK(vkCreateSampler(app->context.device, &samplerInfo, app->allocator,
                             &app->textureSampler),
             "failed to create texture sampler!");
}

void destroy_texture_sampler(App* app)
{
    vkDestroySampler(app->context.device, app->textureSampler, app->allocator);
}

void create_texture_stuff(App* app)
{
    ArrayAtlas atlas = {0};
    atlas_init(&atlas);
    load_texture_into_atlas(&atlas);
    //    debug_print_block_faces();
    create_texture_image_from_atlas(app, &atlas);
    create_texture_image_view(app, atlas.count);
    create_texture_sampler(app);
    atlas_free(&atlas);
}
void destroy_texture_stuff(App* app)
{
    destroy_texture_sampler(app);
    destroy_texture_image_view(app);
    destroy_texture_image(app);
}
