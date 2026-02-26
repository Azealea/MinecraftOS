#include "vk_depth.h"

#include <stddef.h>

#include "vk/vk_swapchain.h"
#include "vk/vk_texture.h"

VkFormat findSupportedFormat(App* app, const VkFormat* candidates,
                             int candidates_size, VkImageTiling tiling,
                             VkFormatFeatureFlags features)
{
    for (int i = 0; i < candidates_size; i++)
    {
        auto format = candidates[i];
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(app->context.physicalDevice, format,
                                            &props);

        if (tiling == VK_IMAGE_TILING_LINEAR
            && (props.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL
                 && (props.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    ASSERT(0, "failed to find supported format!");
    return 0;
}

VkFormat findDepthFormat(App* app)
{
    static const VkFormat candidates[] = { VK_FORMAT_D32_SFLOAT,

                                           VK_FORMAT_D32_SFLOAT_S8_UINT,
                                           VK_FORMAT_D24_UNORM_S8_UINT };
    return findSupportedFormat(app, candidates, 3, VK_IMAGE_TILING_OPTIMAL,
                               VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

void create_depth_resources(App* app)
{
    VkFormat depthFormat = findDepthFormat(app);

    create_image(app, app->swapchain.imageExtent.width,
                 app->swapchain.imageExtent.height, depthFormat,
                 VK_IMAGE_TILING_OPTIMAL,
                 VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &app->depthImage,
                 &app->depthImageMemory);
    app->depthImageView = create_image_view(app, app->depthImage, depthFormat,
                                            VK_IMAGE_ASPECT_DEPTH_BIT);
}

void destroy_depth_resources(App* app)
{
    vkDestroyImageView(app->context.device, app->depthImageView,
                       app->allocator);
    vkDestroyImage(app->context.device, app->depthImage, app->allocator);
    vkFreeMemory(app->context.device, app->depthImageMemory, app->allocator);
}
