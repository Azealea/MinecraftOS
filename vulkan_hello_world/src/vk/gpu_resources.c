#include "vk/gpu_resources.h"

#include "vk/vk_device.h"
#include "vk/vk_swapchain.h"

void gpu_buffer_create(App* app, VkDeviceSize size, VkBufferUsageFlags usage,
                       VkMemoryPropertyFlags props, GpuBuffer* out)
{
    VkBufferCreateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    ASSERTVK(vkCreateBuffer(app->renderer.context.device, &bufferInfo,
                            app->renderer.allocator, &out->buf),
             "Failed to create buffer");

    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(app->renderer.context.device, out->buf, &memReqs);

    VkMemoryAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memReqs.size,
        .memoryTypeIndex = find_memory_type(app, memReqs.memoryTypeBits, props),
    };

    ASSERTVK(vkAllocateMemory(app->renderer.context.device, &allocInfo,
                              app->renderer.allocator, &out->mem),
             "Failed to allocate buffer memory");

    vkBindBufferMemory(app->renderer.context.device, out->buf, out->mem, 0);
}

void gpu_buffer_destroy(App* app, GpuBuffer* buf)
{
    vkDestroyBuffer(app->renderer.context.device, buf->buf, app->renderer.allocator);
    vkFreeMemory(app->renderer.context.device, buf->mem, app->renderer.allocator);
}

void gpu_image_create(App* app, uint32_t width, uint32_t height, uint32_t layers,
                      VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                      VkMemoryPropertyFlags props, VkImageAspectFlags aspect,
                      GpuImage* out)
{
    VkImageCreateInfo imageInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .extent.width = width,
        .extent.height = height,
        .extent.depth = 1,
        .mipLevels = 1,
        .arrayLayers = layers,
        .format = format,
        .tiling = tiling,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .usage = usage,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    ASSERTVK(vkCreateImage(app->renderer.context.device, &imageInfo,
                           app->renderer.allocator, &out->img),
             "Failed to create image");

    VkMemoryRequirements memReqs;
    vkGetImageMemoryRequirements(app->renderer.context.device, out->img, &memReqs);

    VkMemoryAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memReqs.size,
        .memoryTypeIndex = find_memory_type(app, memReqs.memoryTypeBits, props),
    };

    ASSERTVK(vkAllocateMemory(app->renderer.context.device, &allocInfo,
                              app->renderer.allocator, &out->mem),
             "Failed to allocate image memory");

    vkBindImageMemory(app->renderer.context.device, out->img, out->mem, 0);

    out->view = create_image_view(app, out->img, format, aspect, layers);
}

void gpu_image_destroy(App* app, GpuImage* img)
{
    vkDestroyImageView(app->renderer.context.device, img->view, app->renderer.allocator);
    vkDestroyImage(app->renderer.context.device, img->img, app->renderer.allocator);
    vkFreeMemory(app->renderer.context.device, img->mem, app->renderer.allocator);
}
