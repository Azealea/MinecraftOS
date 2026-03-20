#include "vk/vk_swapchain.h"

#include <stdlib.h>

static VkSurfaceCapabilitiesKHR get_surface_capabilities(const App* app)
{
    VkSurfaceCapabilitiesKHR capabilities;
    ASSERTVK(
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            app->context.physicalDevice, app->context.surface, &capabilities),
        "Failed to get surface capabilities");

    return capabilities;
}

static VkSurfaceFormatKHR get_surface_format(App* app)
{
    uint32_t formatCount;
    ASSERTVK(vkGetPhysicalDeviceSurfaceFormatsKHR(app->context.physicalDevice,
                                                  app->context.surface,
                                                  &formatCount, NULL),
             "Couldn't get surface formats");

    VkSurfaceFormatKHR* formats =
        malloc(formatCount * sizeof(VkSurfaceFormatKHR));
    ASSERT(formats != nullptr, "Couldn't allocate formats memory");

    ASSERTVK(vkGetPhysicalDeviceSurfaceFormatsKHR(app->context.physicalDevice,
                                                  app->context.surface,
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
                 app->context.physicalDevice, app->context.surface,
                 &presentModeCount, NULL),
             "Couldn't get surface present modes count");

    VkPresentModeKHR* presentModes =
        malloc(presentModeCount * sizeof(VkPresentModeKHR));

    ASSERT(presentModes != nullptr, "Couldn't allocate present modes memory");

    ASSERTVK(vkGetPhysicalDeviceSurfacePresentModesKHR(
                 app->context.physicalDevice, app->context.surface,
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
get_extent_from_capabilities(const App* app,
                             const VkSurfaceCapabilitiesKHR* capabilities)
{
    const GLFWvidmode* mode = glfwGetVideoMode(app->windowMonitor);
    uint32_t width =
        clamp((uint32_t)mode->width, capabilities->minImageExtent.width,
              capabilities->maxImageExtent.width);

    uint32_t height =
        clamp((uint32_t)mode->height, capabilities->minImageExtent.height,
              capabilities->maxImageExtent.height);
    return (VkExtent2D){.width = width, .height = height};
}

static void get_swapchain_images(App* app)
{
    ASSERTVK(vkGetSwapchainImagesKHR(app->context.device,
                                     app->swapchain.swapchain,
                                     &app->swapchain.imageCount, NULL),
             "Couldn't get swapchain images count");

    app->swapchain.images = malloc(app->swapchain.imageCount * sizeof(VkImage));
    ASSERT(app->swapchain.images != nullptr,
           "Couldn't allocate memory for swapchain images");

    ASSERTVK(vkGetSwapchainImagesKHR(
                 app->context.device, app->swapchain.swapchain,
                 &app->swapchain.imageCount, app->swapchain.images),
             "Couldn't get swapchain images");
}

VkImageView create_image_view(App* app, VkImage image, VkFormat format,
                              VkImageAspectFlags aspectFlags)
{
    VkImageViewCreateInfo viewInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .subresourceRange.aspectMask = aspectFlags,
        .subresourceRange.baseMipLevel = 0,
        .subresourceRange.levelCount = 1,
        .subresourceRange.baseArrayLayer = 0,
        .subresourceRange.layerCount = 1,
    };

    VkImageView imageView;
    ASSERTVK(vkCreateImageView(app->context.device, &viewInfo, app->allocator,
                               &imageView),
             "failed to create image view!")

    return imageView;
}

static void create_swapchain_image_views(App* app)
{
    app->swapchain.imageViews =
        malloc(app->swapchain.imageCount * sizeof(VkImageView));
    ASSERT(app->swapchain.imageViews != nullptr,
           "Couldn't allocate memory for swapchain image views");

    for (uint32_t i = 0; i < app->swapchain.imageCount; ++i)
    {
        app->swapchain.imageViews[i] =
            create_image_view(app, app->swapchain.images[i],
                              app->swapchain.format, VK_IMAGE_ASPECT_COLOR_BIT);
    }
}

void create_swapchain(App* app)
{
    VkSurfaceCapabilitiesKHR capabilities = get_surface_capabilities(app);

    app->swapchain.imageExtent =
        get_extent_from_capabilities(app, &capabilities);

    VkSurfaceFormatKHR surface_format = get_surface_format(app);
    app->swapchain.colorSpace = surface_format.colorSpace;
    app->swapchain.format = surface_format.format;

    ASSERTVK(vkCreateSwapchainKHR(
                 app->context.device,
                 &(VkSwapchainCreateInfoKHR){
                     .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                     .surface = app->context.surface,
                     .queueFamilyIndexCount = 1,
                     .pQueueFamilyIndices = &app->context.queueFamily,
                     .clipped = true,
                     .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                     .imageArrayLayers = 1,
                     .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                     .preTransform = capabilities.currentTransform,
                     .imageExtent = app->swapchain.imageExtent,
                     .imageFormat = app->swapchain.format,
                     .imageColorSpace = app->swapchain.colorSpace,
                     .presentMode = get_present_mode(app),
                     .minImageCount = clamp(3, capabilities.minImageCount,
                                            capabilities.maxImageCount
                                                ? capabilities.maxImageCount
                                                : UINT32_MAX),
                 },
                 app->allocator, &app->swapchain.swapchain),
             "Couldn't create swapchain");

    get_swapchain_images(app);
    create_swapchain_image_views(app);
}

void destroy_swapchain(App* app)
{
    for (uint32_t i = 0; i < app->swapchain.imageCount; ++i)
    {
        vkDestroyImageView(app->context.device, app->swapchain.imageViews[i],
                           app->allocator);
    }
    free(app->swapchain.imageViews);
    free(app->swapchain.images);

    vkDestroySwapchainKHR(app->context.device, app->swapchain.swapchain,
                          app->allocator);
}

uint32_t acquire_swapchain_image(App* app, uint32_t frameIndex)
{
    uint32_t acquired_index;

    VkResult result = vkAcquireNextImageKHR(
        app->context.device, app->swapchain.swapchain, UINT64_MAX,
        app->renderer.imageAvailableSemaphores[frameIndex], VK_NULL_HANDLE,
        &acquired_index);

    ASSERTVK(result, "Failed to acquire swapchain image (result = %d)", result);
    return acquired_index;
}

void present_swapchain_image(App* app, uint32_t imageIndex)
{
    VkSwapchainKHR swapchains[] = {app->swapchain.swapchain};

    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores =
            (VkSemaphore[]){app->renderer.renderFinishedSemaphores[imageIndex]},
        .swapchainCount = 1,
        .pSwapchains = swapchains,
        .pImageIndices = &imageIndex,
        .pResults = NULL,
    };

    VkResult result = vkQueuePresentKHR(app->context.queue, &presentInfo);
    ASSERTVK(result, "Failed to present swapchain image (result = %d)", result);
}
