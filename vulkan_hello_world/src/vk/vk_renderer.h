#pragma once

#include <vulkan/vulkan.h>

typedef struct
{
    VkBuffer buf;
    VkDeviceMemory mem;
} GpuBuffer;
typedef struct
{
    VkImage img;
    VkDeviceMemory mem;
    VkImageView view;
} GpuImage;

typedef struct Renderer
{
    VkAllocationCallbacks* allocator;

    struct
    {
        VkInstance instance;
        VkSurfaceKHR surface;
        VkPhysicalDevice physicalDevice;
        uint32_t queueFamily;
        VkDevice device;
        VkQueue queue;
    } context;

    struct
    {
        VkSwapchainKHR handle;
        uint32_t imageCount;
        VkImage* images; // [imageCount]
        VkImageView* imageViews; // [imageCount]
        VkFormat format;
        VkColorSpaceKHR colorSpace;
        VkExtent2D extent;
    } swapchain;

    struct
    {
        VkRenderPass renderpass;
        VkDescriptorSetLayout descriptorSetLayout;
        VkPipelineLayout layout;
        VkPipeline graphics;
        VkFramebuffer* framebuffers; // [imageCount]
    } pipeline;

    struct
    {
        VkCommandPool pool;
        VkCommandBuffer* buffers; // [maxFramesInFlight]
        VkSemaphore* imageAvailable; // [maxFramesInFlight]
        VkSemaphore* renderFinished; // [imageCount]
        VkFence* inFlight; // [maxFramesInFlight]
    } sync;

    struct
    {
        GpuBuffer vertex;
        GpuBuffer* uniforms; // [maxFramesInFlight]
        void** uniformsMapped; // [maxFramesInFlight]
    } buffers;

    struct
    {
        VkDescriptorPool pool;
        VkDescriptorSet* sets; // [maxFramesInFlight]
    } descriptors;

    struct
    {
        GpuImage image;
        VkSampler sampler;
    } texture;

    GpuImage depth;
} Renderer;
