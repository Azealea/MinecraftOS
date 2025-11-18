#include <stdlib.h>

#include "app.h"

static VkSurfaceCapabilitiesKHR get_surface_capabilities(const App* app)
{
    VkSurfaceCapabilitiesKHR capabilities;
    ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
               app->context.physicalDevice, app->context.surface, &capabilities)
               == VK_SUCCESS,
           "Failed to get surface capabilities");

    return capabilities;
}

static VkSurfaceFormatKHR get_surface_format(App* app)
{
    uint32_t formatCount;
    ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(app->context.physicalDevice,
                                                app->context.surface,
                                                &formatCount, NULL)
               == VK_SUCCESS,
           "Couldn't get surface formats");

    VkSurfaceFormatKHR* formats =
        malloc(formatCount * sizeof(VkSurfaceFormatKHR));
    ASSERT(formats != nullptr, "Couldn't allocate formats memory");

    ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(app->context.physicalDevice,
                                                app->context.surface,
                                                &formatCount, formats)
               == VK_SUCCESS,
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
    ASSERT(vkGetPhysicalDeviceSurfacePresentModesKHR(
               app->context.physicalDevice, app->context.surface,
               &presentModeCount, NULL)
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
    return (VkExtent2D){ .width = width, .height = height };
}

static void get_swapchain_images(App* app)
{
    ASSERT(vkGetSwapchainImagesKHR(app->context.device,
                                   app->swapchain.swapchain,
                                   &app->swapchain.swapchainImageCount, NULL)
               == VK_SUCCESS,
           "Couldn't get swapchain images count");

    app->swapchain.swapchainImages =
        malloc(app->swapchain.swapchainImageCount * sizeof(VkImage));
    ASSERT(app->swapchain.swapchainImages != nullptr,
           "Couldn't allocate memory for swapchain images");

    ASSERT(vkGetSwapchainImagesKHR(app->context.device,
                                   app->swapchain.swapchain,
                                   &app->swapchain.swapchainImageCount,
                                   app->swapchain.swapchainImages)
               == VK_SUCCESS,
           "Couldn't get swapchain images");
}

static void create_swapchain_image_views(App* app)
{
    app->swapchain.swapchainImageViews =
        malloc(app->swapchain.swapchainImageCount * sizeof(VkImageView));
    ASSERT(app->swapchain.swapchainImageViews != nullptr,
           "Couldn't allocate memory for swapchain image views");

    for (uint32_t i = 0; i < app->swapchain.swapchainImageCount; ++i)
    {
        ASSERT(vkCreateImageView(
                   app->context.device,
                   &(VkImageViewCreateInfo){
                       .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                       .format = app->swapchain.format,
                       .image = app->swapchain.swapchainImages[i],
                       .components = (VkComponentMapping){},
                       .subresourceRange =
                           (VkImageSubresourceRange){
                               .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                               .layerCount = 1,
                               .levelCount = 1,
                           },
                       .viewType = VK_IMAGE_VIEW_TYPE_2D,
                   },
                   app->allocator, &app->swapchain.swapchainImageViews[i])
                   == VK_SUCCESS,
               "Couldn't create image view %i", i);
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

    ASSERT(vkCreateSwapchainKHR(
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
               app->allocator, &app->swapchain.swapchain)
               == VK_SUCCESS,
           "Couldn't create swapchain");

    get_swapchain_images(app);
    create_swapchain_image_views(app);
}

void destroy_swapchain(App* app)
{
    for (uint32_t i = 0; i < app->swapchain.swapchainImageCount; ++i)
    {
        vkDestroyImageView(app->context.device,
                           app->swapchain.swapchainImageViews[i],
                           app->allocator);
    }
    free(app->swapchain.swapchainImageViews);
    free(app->swapchain.swapchainImages);

    vkDestroySwapchainKHR(app->context.device, app->swapchain.swapchain,
                          app->allocator);
}