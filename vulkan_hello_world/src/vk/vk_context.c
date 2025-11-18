#include "vk/vk.h"
#include "vk/vk_device.h"

static void create_instance(App* app)
{
    uint32_t requiredExtensionsCount;
    const char** requiredExtensions =
        glfwGetRequiredInstanceExtensions(&requiredExtensionsCount);

    ASSERT(vkCreateInstance(
               &(VkInstanceCreateInfo){
                   .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                   .pApplicationInfo =
                       &(VkApplicationInfo){
                           .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                           .apiVersion = app->vkApiVersion,
                           .pApplicationName = app->applicationName,
                           .pEngineName = app->engineName,
                       },
                   .enabledExtensionCount = requiredExtensionsCount,
                   .ppEnabledExtensionNames = requiredExtensions,
               },
               app->allocator, &app->context.instance)
               == VK_SUCCESS,
           "Couldn't create instance")
}

static void create_surface(App* app)
{
    ASSERT(glfwCreateWindowSurface(app->context.instance, app->window,
                                   app->allocator, &app->context.surface)
               == VK_SUCCESS,
           "Couldn't create window surface");
}

void create_vk_context(App* app)
{
    create_instance(app);

    select_physical_device(app);
    select_queue_family(app);
    create_device(app);
    get_queue(app);

    create_surface(app);
}

void destroy_vk_context(App* app)
{
    vkDestroySurfaceKHR(app->context.instance, app->context.surface,
                        app->allocator);
    vkDestroyDevice(app->context.device, app->allocator);
    vkDestroyInstance(app->context.instance, app->allocator);
}
