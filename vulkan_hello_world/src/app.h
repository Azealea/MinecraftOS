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
    int windowWidth;
    int windowHeight;
    bool windowResizable;
    bool windowFullscreen;

    // GLFW
    GLFWmonitor* windowMonitor;
    GLFWwindow* window;

    // Vulkan
    uint32_t apiVersion;
    VkAllocationCallbacks* allocator;

    VkInstance instance;
    uint32_t queueFamily;

    VkPhysicalDevice physicalDevice;
    VkSurfaceKHR surface;
    VkDevice device;
    VkQueue queue;

} App;
