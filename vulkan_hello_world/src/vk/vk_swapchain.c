#include "vk/vk_swapchain.h"

#include <stdlib.h>

static VkSurfaceCapabilitiesKHR get_surface_capabilities(const App* app)
{
    VkSurfaceCapabilitiesKHR capabilities;
    ASSERTVK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
                 app->renderer.context.physicalDevice, app->renderer.context.surface,
                 &capabilities),
             "Failed to get surface capabilities");

    return capabilities;
}

static VkSurfaceFormatKHR get_surface_format(App* app)
{
    uint32_t formatCount;
    ASSERTVK(vkGetPhysicalDeviceSurfaceFormatsKHR(app->renderer.context.physicalDevice,
                                                  app->renderer.context.surface,
                                                  &formatCount, NULL),
             "Couldn't get surface formats");

    VkSurfaceFormatKHR* formats = malloc(formatCount * sizeof(VkSurfaceFormatKHR));
    ASSERT(formats != nullptr, "Couldn't allocate formats memory");

    ASSERTVK(vkGetPhysicalDeviceSurfaceFormatsKHR(app->renderer.context.physicalDevice,
                                                  app->renderer.context.surface,
                                                  &formatCount, formats),
             "Couldn't get surface formats");

    VkSurfaceFormatKHR surface_format = formats[0];
    for (uint32_t i = 0; i < formatCount; ++i)
    {
        if (formats[i].colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR
            && formats[i].format == VK_FORMAT_B8G8R8A8_SRGB)
        {
            surface_format = formats[i];
            break;
        }
    }

    free(formats);
    return surface_format;
}

static VkPresentModeKHR get_present_mode(App* app)
{
    uint32_t presentModeCount;
    ASSERTVK(vkGetPhysicalDeviceSurfacePresentModesKHR(
                 app->renderer.context.physicalDevice, app->renderer.context.surface,
                 &presentModeCount, NULL),
             "Couldn't get surface present modes count");

    VkPresentModeKHR* presentModes = malloc(presentModeCount * sizeof(VkPresentModeKHR));

    ASSERT(presentModes != nullptr, "Couldn't allocate present modes memory");

    ASSERTVK(vkGetPhysicalDeviceSurfacePresentModesKHR(
                 app->renderer.context.physicalDevice, app->renderer.context.surface,
                 &presentModeCount, presentModes),
             "Couldn't get surface present modes");

    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    // here by default i think

    for (uint32_t i = 0; i < presentModeCount; ++i)
    {
        if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            presentMode = presentModes[i];
            break;
        }
    }

    free(presentModes);
    return presentMode;
}

static VkExtent2D
get_extent_from_capabilities(const App* app, const VkSurfaceCapabilitiesKHR* capabilities)
{
    const GLFWvidmode* mode = glfwGetVideoMode(app->windowMonitor);
    uint32_t width = clamp((uint32_t)mode->width, capabilities->minImageExtent.width,
                           capabilities->maxImageExtent.width);

    uint32_t height = clamp((uint32_t)mode->height, capabilities->minImageExtent.height,
                            capabilities->maxImageExtent.height);
    return (VkExtent2D){.width = width, .height = height};
}

static void get_swapchain_images(App* app)
{
    ASSERTVK(vkGetSwapchainImagesKHR(app->renderer.context.device,
                                     app->renderer.swapchain.handle,
                                     &app->renderer.swapchain.imageCount, NULL),
             "Couldn't get swapchain images count");

    app->renderer.swapchain.images =
        malloc(app->renderer.swapchain.imageCount * sizeof(VkImage));
    ASSERT(app->renderer.swapchain.images != nullptr,
           "Couldn't allocate memory for swapchain images");

    ASSERTVK(vkGetSwapchainImagesKHR(
                 app->renderer.context.device, app->renderer.swapchain.handle,
                 &app->renderer.swapchain.imageCount, app->renderer.swapchain.images),
             "Couldn't get swapchain images");
}

VkImageView create_image_view(App* app, VkImage image, VkFormat format,
                              VkImageAspectFlags aspectFlags, uint32_t layers)
{
    VkImageViewCreateInfo viewInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = image,
        .viewType = layers > 1 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .subresourceRange.aspectMask = aspectFlags,
        .subresourceRange.baseMipLevel = 0,
        .subresourceRange.levelCount = 1,
        .subresourceRange.baseArrayLayer = 0,
        .subresourceRange.layerCount = layers,
    };

    VkImageView imageView;
    ASSERTVK(vkCreateImageView(app->renderer.context.device, &viewInfo,
                               app->renderer.allocator, &imageView),
             "failed to create image view!")

    return imageView;
}

static void create_swapchain_image_views(App* app)
{
    app->renderer.swapchain.imageViews =
        malloc(app->renderer.swapchain.imageCount * sizeof(VkImageView));
    ASSERT(app->renderer.swapchain.imageViews != nullptr,
           "Couldn't allocate memory for swapchain image views");

    for (uint32_t i = 0; i < app->renderer.swapchain.imageCount; ++i)
    {
        app->renderer.swapchain.imageViews[i] = create_image_view(
            app, app->renderer.swapchain.images[i], app->renderer.swapchain.format,
            VK_IMAGE_ASPECT_COLOR_BIT, 1);
    }
}

void create_swapchain(App* app)
{
    VkSurfaceCapabilitiesKHR capabilities = get_surface_capabilities(app);

    app->renderer.swapchain.extent = get_extent_from_capabilities(app, &capabilities);

    VkSurfaceFormatKHR surface_format = get_surface_format(app);
    app->renderer.swapchain.colorSpace = surface_format.colorSpace;
    app->renderer.swapchain.format = surface_format.format;

    ASSERTVK(
        vkCreateSwapchainKHR(
            app->renderer.context.device,
            &(VkSwapchainCreateInfoKHR){
                .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                .surface = app->renderer.context.surface,
                .queueFamilyIndexCount = 1,
                .pQueueFamilyIndices = &app->renderer.context.queueFamily,
                .clipped = true,
                .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                .imageArrayLayers = 1,
                .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                .preTransform = capabilities.currentTransform,
                .imageExtent = app->renderer.swapchain.extent,
                .imageFormat = app->renderer.swapchain.format,
                .imageColorSpace = app->renderer.swapchain.colorSpace,
                .presentMode = get_present_mode(app),
                .minImageCount = clamp(
                    3, capabilities.minImageCount,
                    capabilities.maxImageCount ? capabilities.maxImageCount : UINT32_MAX),
            },
            app->renderer.allocator, &app->renderer.swapchain.handle),
        "Couldn't create swapchain");

    get_swapchain_images(app);
    create_swapchain_image_views(app);
}

void destroy_swapchain(App* app)
{
    for (uint32_t i = 0; i < app->renderer.swapchain.imageCount; ++i)
    {
        vkDestroyImageView(app->renderer.context.device,
                           app->renderer.swapchain.imageViews[i],
                           app->renderer.allocator);
    }
    free(app->renderer.swapchain.imageViews);
    free(app->renderer.swapchain.images);

    vkDestroySwapchainKHR(app->renderer.context.device, app->renderer.swapchain.handle,
                          app->renderer.allocator);
}

uint32_t acquire_swapchain_image(App* app, uint32_t frameIndex)
{
    uint32_t acquired_index;

    VkResult result = vkAcquireNextImageKHR(
        app->renderer.context.device, app->renderer.swapchain.handle, UINT64_MAX,
        app->renderer.sync.imageAvailable[frameIndex], VK_NULL_HANDLE, &acquired_index);

    ASSERTVK(result, "Failed to acquire swapchain image (result = %d)", result);
    return acquired_index;
}

void present_swapchain_image(App* app, uint32_t imageIndex)
{
    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = (VkSemaphore[]){app->renderer.sync.renderFinished[imageIndex]},
        .swapchainCount = 1,
        .pSwapchains = &app->renderer.swapchain.handle,
        .pImageIndices = &imageIndex,
        .pResults = NULL,
    };

    VkResult result = vkQueuePresentKHR(app->renderer.context.queue, &presentInfo);
    ASSERTVK(result, "Failed to present swapchain image (result = %d)", result);
}
