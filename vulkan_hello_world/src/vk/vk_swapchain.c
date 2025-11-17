#include <stdlib.h>

#include "app.h"

static VkSurfaceFormatKHR get_surface_format(App* app)
{
    uint32_t formatCount;
    ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(
               app->physicalDevice, app->surface, &formatCount, NULL)
               == VK_SUCCESS,
           "Couldn't get surface formats");

    VkSurfaceFormatKHR* formats =
        malloc(formatCount * sizeof(VkSurfaceFormatKHR));
    ASSERT(formats != nullptr, "Couldn't allocate formats memory");

    ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(
               app->physicalDevice, app->surface, &formatCount, formats)
               == VK_SUCCESS,
           "Couldn't get surface formats");

    VkSurfaceFormatKHR format = formats[0];
    for (uint32_t i = 0; i < formatCount; ++i)
    {
        if (formats[i].colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR
            && formats[i].format == VK_FORMAT_B8G8R8A8_SRGB)
        {
            format = formats[i];
            break;
        }
    }

    free(formats);
    return format;
}

static VkPresentModeKHR get_present_mode(App* app)
{
    uint32_t presentModeCount;
    ASSERT(vkGetPhysicalDeviceSurfacePresentModesKHR(
               app->physicalDevice, app->surface, &presentModeCount, NULL)
               == VK_SUCCESS,
           "Couldn't get surface present modes count");

    VkPresentModeKHR* presentModes =
        malloc(presentModeCount * sizeof(VkPresentModeKHR));
    ASSERT(presentModes != nullptr, "Couldn't allocate present modes memory");

    // here by default i think
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
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

void create_swapchain(App* app)
{
    VkSurfaceCapabilitiesKHR capabilities;
    ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
               app->physicalDevice, app->surface, &capabilities)
               == VK_SUCCESS,
           "Failed to get surface capabilities");

    VkSurfaceFormatKHR format = get_surface_format(app);
    VkPresentModeKHR presentMode = get_present_mode(app);

    const GLFWvidmode* mode = glfwGetVideoMode(app->windowMonitor);
    uint32_t width =
        clamp((uint32_t)mode->width, capabilities.minImageExtent.width,
              capabilities.maxImageExtent.width);

    uint32_t height =
        clamp((uint32_t)mode->height, capabilities.minImageExtent.height,
              capabilities.maxImageExtent.height);
    VkExtent2D extent = { .width = width, .height = height };

    ASSERT(vkCreateSwapchainKHR(
               app->device,
               &(VkSwapchainCreateInfoKHR){
                   .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                   .surface = app->surface,
                   .queueFamilyIndexCount = 1,
                   .pQueueFamilyIndices = &app->queueFamily,
                   .clipped = true,
                   .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                   .imageArrayLayers = 1,
                   .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                   .preTransform = capabilities.currentTransform,
                   .imageExtent = extent,
                   .imageFormat = format.format,
                   .imageColorSpace = format.colorSpace,
                   .presentMode = presentMode,
                   .minImageCount = clamp(3, capabilities.minImageCount,
                                          capabilities.maxImageCount
                                              ? capabilities.maxImageCount
                                              : UINT32_MAX),
               },
               app->allocator, &app->sc.swapchain)
               == VK_SUCCESS,
           "Couldn't create swapchain");

    ASSERT(vkGetSwapchainImagesKHR(app->device, app->sc.swapchain,
                                   &app->sc.swapchainImageCount, NULL)
               == VK_SUCCESS,
           "Couldn't get swapchain images count");

    app->sc.swapchainImages =
        malloc(app->sc.swapchainImageCount * sizeof(VkImage));
    ASSERT(app->sc.swapchainImages != nullptr,
           "Couldn't allocate memory for swapchain images");

    ASSERT(vkGetSwapchainImagesKHR(app->device, app->sc.swapchain,
                                   &app->sc.swapchainImageCount,
                                   app->sc.swapchainImages)
               == VK_SUCCESS,
           "Couldn't get swapchain images");

    app->sc.swapchainImageViews =
        malloc(app->sc.swapchainImageCount * sizeof(VkImageView));
    ASSERT(app->sc.swapchainImageViews != nullptr,
           "Couldn't allocate memory for swapchain image views");

    for (uint32_t i = 0; i < app->sc.swapchainImageCount; ++i)
    {
        ASSERT(vkCreateImageView(
                   app->device,
                   &(VkImageViewCreateInfo){
                       .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                       .format = format.format,
                       .image = app->sc.swapchainImages[i],
                       .components = (VkComponentMapping){},
                       .subresourceRange =
                           (VkImageSubresourceRange){
                               .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                               .layerCount = 1,
                               .levelCount = 1,
                           },
                       .viewType = VK_IMAGE_VIEW_TYPE_2D,
                   },
                   app->allocator, &app->sc.swapchainImageViews[i])
                   == VK_SUCCESS,
               "Couldn't create image view %i", i);
    }
}