#include "vk/vk_device.h"

#include <stdlib.h>

void select_physical_device(App* app)
{
    uint32_t count;
    ASSERTVK(vkEnumeratePhysicalDevices(app->renderer.context.instance, &count,
                                        NULL),
             "Couldn't enumerate physical devices count");
    ASSERT(count != 0, "Couldn't find a vulkan supported physical device");
    VkResult res = vkEnumeratePhysicalDevices(
        app->renderer.context.instance, &(uint32_t){1},
        &app->renderer.context.physicalDevice);
    ASSERT(res == VK_SUCCESS || res == VK_INCOMPLETE,
           "Couldn't enumerate physical devices count");

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(app->renderer.context.physicalDevice,
                                &supportedFeatures);

    ASSERT(supportedFeatures.samplerAnisotropy, "no anisotropy support!");
}

void select_queue_family(App* app)
{
    uint32_t count;
    vkGetPhysicalDeviceQueueFamilyProperties(
        app->renderer.context.physicalDevice, &count, NULL);

    ASSERT(count != 0, "no family queue?");
    VkQueueFamilyProperties* queueFamilies =
        malloc(count * sizeof(VkQueueFamilyProperties));
    ASSERT(queueFamilies, "Couldn't allocate memory");
    vkGetPhysicalDeviceQueueFamilyProperties(
        app->renderer.context.physicalDevice, &count, queueFamilies);

    app->renderer.context.queueFamily = UINT32_MAX;
    for (uint32_t queueFamilyIndex = 0; queueFamilyIndex < count;
         ++queueFamilyIndex)
    {
        VkQueueFamilyProperties properties = queueFamilies[queueFamilyIndex];

        if ((properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            && glfwGetPhysicalDevicePresentationSupport(
                app->renderer.context.instance,
                app->renderer.context.physicalDevice, queueFamilyIndex))
        {
            app->renderer.context.queueFamily = queueFamilyIndex;
            break;
        }
    }

    ASSERT(app->renderer.context.queueFamily != UINT32_MAX,
           "Couldn't find a suitable queue family");
    free(queueFamilies);
}

void create_device(App* app)
{
    ASSERTVK(
        vkCreateDevice(
            app->renderer.context.physicalDevice,
            &(VkDeviceCreateInfo){
                .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                .pQueueCreateInfos =
                    &(VkDeviceQueueCreateInfo){
                        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                        .queueFamilyIndex = app->renderer.context.queueFamily,
                        .queueCount = 1,
                        .pQueuePriorities = &(float){1.0},
                    },
                .queueCreateInfoCount = 1,
                .enabledExtensionCount = 1,
                .ppEnabledExtensionNames =
                    &(const char*){VK_KHR_SWAPCHAIN_EXTENSION_NAME},
                .pEnabledFeatures =
                    &(VkPhysicalDeviceFeatures){
                        .samplerAnisotropy = VK_TRUE,
                    },
            },
            app->renderer.allocator, &app->renderer.context.device),
        "Couldn't create device and queues");
}

void get_queue(App* app)
{
    vkGetDeviceQueue(app->renderer.context.device,
                     app->renderer.context.queueFamily, 0,
                     &app->renderer.context.queue);
}

uint32_t find_memory_type(App* app, uint32_t typeFilter,
                          VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(app->renderer.context.physicalDevice,
                                        &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1u << i))
            && (memProperties.memoryTypes[i].propertyFlags & properties)
                == properties)
        {
            return i;
        }
    }

    ASSERT(0, "No suitable memory type\n");
    return 0;
}
