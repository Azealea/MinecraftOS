#include <stdio.h>
#include <stdlib.h>

#include "app.h"
#include "glfw.h"
#include "vk/vk.h"

static void display_info()
{
    uint32_t instanceApiVersion;
    ASSERT(vkEnumerateInstanceVersion(&instanceApiVersion) == VK_SUCCESS,
           "Couldn't enumerate instance version");
    uint32_t apiVersionVariant = VK_API_VERSION_VARIANT(instanceApiVersion);
    uint32_t apiVersionMajor = VK_API_VERSION_MAJOR(instanceApiVersion);
    uint32_t apiVersionMinor = VK_API_VERSION_MINOR(instanceApiVersion);
    uint32_t apiVersionPatch = VK_API_VERSION_PATCH(instanceApiVersion);
    printf("Vulkan API %i.%i.%i.%i\n", apiVersionVariant, apiVersionMajor,
           apiVersionMinor, apiVersionPatch);
    printf("GLFW %s\n", glfwGetVersionString());
}

static void init(App* app)
{
    display_info();
    init_glfw(app);
    create_glfw_window(app);
    init_vk(app);
}

static void loop(App* app)
{
    while (!glfwWindowShouldClose(app->window))
    {
        glfwPollEvents();
    }
}

static void cleanup(App* app)
{
    vkDestroyDevice(app->device, app->allocator);
    vkDestroySurfaceKHR(app->instance, app->surface, app->allocator);
    vkDestroyInstance(app->instance, app->allocator);
}

int main()
{
    App app = {
        .applicationName = "hello_world",
        .engineName = "SUN ENGINE",
        .windowTitle = "hello",
        .windowWidth = 720,
        .windowHeight = 480,
        .windowResizable = false,
        .windowFullscreen = true,
        .apiVersion = VK_API_VERSION_1_4,
    };

    init(&app);
    loop(&app);
    cleanup(&app);

    return EXIT_SUCCESS;
}
