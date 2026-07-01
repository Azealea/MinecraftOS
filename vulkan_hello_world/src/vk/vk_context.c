#include "vk/vk_context.h"

#include "vk/vk_device.h"

static void create_instance(App* app)
{
    uint32_t requiredExtensionsCount;
    const char** requiredExtensions =
        glfwGetRequiredInstanceExtensions(&requiredExtensionsCount);

    ASSERTVK(vkCreateInstance(
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
                 app->renderer.allocator, &app->renderer.context.instance),
             "Couldn't create instance")
}

static void create_surface(App* app)
{
    ASSERTVK(glfwCreateWindowSurface(app->renderer.context.instance,
                                     app->window, app->renderer.allocator,
                                     &app->renderer.context.surface),
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
    vkDestroySurfaceKHR(app->renderer.context.instance,
                        app->renderer.context.surface, app->renderer.allocator);
    vkDestroyDevice(app->renderer.context.device, app->renderer.allocator);
    vkDestroyInstance(app->renderer.context.instance, app->renderer.allocator);
}
