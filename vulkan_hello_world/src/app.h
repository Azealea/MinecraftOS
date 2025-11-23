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
    VkClearValue backgroundColor;
    uint32_t maxFramesInFlight;

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

        uint32_t imageCount;
        VkImage* images;
        VkImageView* imageViews;

        VkFormat format;
        VkColorSpaceKHR colorSpace;
        VkExtent2D imageExtent;
    } swapchain;

    struct renderer
    {
        VkPipeline graphicsPipeline;
        VkRenderPass renderpass;
        VkFramebuffer* framebuffers;

        VkCommandPool commandPool;
        // arrays of size maxFramesInFlight
        VkCommandBuffer* commandBuffers;
        VkSemaphore* imageAvailableSemaphores;
        VkSemaphore* renderFinishedSemaphores;
        VkFence* inFlightFences;
    } renderer;

} App;