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
    init_glfw();
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
    for (uint32_t i = 0; i < app->sc.swapchainImageCount; ++i)
    {
        vkDestroyImageView(app->device, app->sc.swapchainImageViews[i],
                           app->allocator);
    }
    free(app->sc.swapchainImageViews);
    free(app->sc.swapchainImages);

    vkDestroySwapchainKHR(app->device, app->sc.swapchain, app->allocator);
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
        .windowFullscreen = true,
        .apiVersion = VK_API_VERSION_1_4,
    };

    init(&app);
    loop(&app);
    cleanup(&app);

    return EXIT_SUCCESS;
}
