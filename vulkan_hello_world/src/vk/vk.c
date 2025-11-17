#include "vk/vk.h"

#include "vk/vk_device.h"
#include "vk/vk_swapchain.h"

static void create_instance(App* app)
{
    uint32_t requiredExtensionsCount;
    const char** requiredExtensions =
        glfwGetRequiredInstanceExtensions(&requiredExtensionsCount);

    ASSERT(VK_SUCCESS
               == vkCreateInstance(
                   &(VkInstanceCreateInfo){
                       .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                       .pApplicationInfo =
                           &(VkApplicationInfo){
                               .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                               .apiVersion = app->apiVersion,
                               .pApplicationName = app->applicationName,
                               .pEngineName = app->engineName,
                           },
                       .enabledExtensionCount = requiredExtensionsCount,
                       .ppEnabledExtensionNames = requiredExtensions,
                   },
                   app->allocator, &app->instance),
           "Couldn't create instance")
}

static void create_surface(App* app)
{
    ASSERT(glfwCreateWindowSurface(app->instance, app->window, app->allocator,
                                   &app->surface)
               == VK_SUCCESS,
           "Couldn't create window surface");
}

void init_vk(App* app)
{
    create_instance(app);

    select_physical_device(app);
    create_surface(app);
    select_queue_family(app);
    create_device(app);
    get_queue(app);

    create_swapchain(app);
}
