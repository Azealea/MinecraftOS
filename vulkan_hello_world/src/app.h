#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "utils.h"

typedef struct
{
    // Configurable
    const char* applicationName;
    const char* engineName;
    const char* windowTitle;
    uint32_t vkApiVersion;
    VkAllocationCallbacks* allocator;

    // GLFW
    GLFWmonitor* windowMonitor;
    GLFWwindow* window;

    // Vulkan
    struct context
    {
        VkInstance instance;
        VkSurfaceKHR surface;

        VkPhysicalDevice physicalDevice;
        uint32_t queueFamily;
        VkDevice device;
        VkQueue queue;
    } context;

    struct swapchain
    {
        VkSwapchainKHR swapchain;
        uint32_t swapchainImageCount;
        VkImage* swapchainImages;
        VkImageView* swapchainImageViews;

        VkFormat format;
        VkColorSpaceKHR colorSpace;
        VkExtent2D imageExtent;
    } swapchain;

    struct renderer
    {
        VkPipeline graphicsPipeline;
        VkPipelineLayout pipelineLayout;
    } renderer;

} App;
