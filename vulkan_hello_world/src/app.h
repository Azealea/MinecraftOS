#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "utils/utils.h"
#include "vk/camera.h"
#include "world.h"

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
    double start_time;

    // GLFW
    GLFWmonitor* windowMonitor;
    GLFWwindow* window;

    Camera camera;

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
        VkDescriptorSetLayout descriptorSetLayout;
        VkPipelineLayout pipelineLayout;

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

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    // arrays of size maxFramesInFlight
    VkBuffer* uniformBuffers;
    VkDeviceMemory* uniformBuffersMemory;
    void** uniformBuffersMapped;

    VkDescriptorPool descriptorPool;
    VkDescriptorSet* descriptorSets;

    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    VkSampler textureSampler;

    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;

    World world;
} App;
