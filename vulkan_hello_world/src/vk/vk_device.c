#include <stdlib.h>

#include "app.h"

void select_physical_device(App* app)
{
    uint32_t count;
    ASSERT(vkEnumeratePhysicalDevices(app->instance, &count, NULL)
               == VK_SUCCESS,
           "Couldn't enumerate physical devices count");
    ASSERT(count != 0, "Couldn't find a vulkan supported physical device");
    ASSERT(vkEnumeratePhysicalDevices(app->instance, &(uint32_t){ 1 },
                                      &app->physicalDevice)
               == VK_SUCCESS,
           "Couldn't enumerate physical devices count");
}

void select_queue_family(App* app)
{
    uint32_t count;
    vkGetPhysicalDeviceQueueFamilyProperties(app->physicalDevice, &count, NULL);

    ASSERT(count != 0, "no family queue?");
    VkQueueFamilyProperties* queueFamilies =
        malloc(count * sizeof(VkQueueFamilyProperties));
    ASSERT(queueFamilies, "Couldn't allocate memory");
    vkGetPhysicalDeviceQueueFamilyProperties(app->physicalDevice, &count,
                                             queueFamilies);

    app->queueFamily = UINT32_MAX;
    for (uint32_t queueFamilyIndex = 0; queueFamilyIndex < count;
         ++queueFamilyIndex)
    {
        VkQueueFamilyProperties properties = queueFamilies[queueFamilyIndex];

        if ((properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            && glfwGetPhysicalDevicePresentationSupport(
                app->instance, app->physicalDevice, queueFamilyIndex))
        {
            app->queueFamily = queueFamilyIndex;
            break;
        }
    }

    ASSERT(app->queueFamily != UINT32_MAX,
           "Couldn't find a suitable queue family");
    free(queueFamilies);
}

void create_device(App* app)
{
    ASSERT(vkCreateDevice(
               app->physicalDevice,
               &(VkDeviceCreateInfo){
                   .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                   .pQueueCreateInfos =
                       &(VkDeviceQueueCreateInfo){
                           .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                           .queueFamilyIndex = app->queueFamily,
                           .queueCount = 1,
                           .pQueuePriorities = &(float){ 1.0 },
                       },
                   .queueCreateInfoCount = 1,
                   .enabledExtensionCount = 1,
                   .ppEnabledExtensionNames =
                       &(const char*){ VK_KHR_SWAPCHAIN_EXTENSION_NAME },
               },
               app->allocator, &app->device)
               == VK_SUCCESS,
           "Couldn't create device and queues");
}

void get_queue(App* app)
{
    vkGetDeviceQueue(app->device, app->queueFamily, 0, &app->queue);
}
