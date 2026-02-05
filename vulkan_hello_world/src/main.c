#include <stdio.h>
#include <stdlib.h>

#include "app.h"
#include "glfw.h"
#include "utils/time.h"
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
        double frameTimeStart = glfwGetTime();
        glfwPollEvents();
        do_stuff_vk(app);
        double frameTimeEnd = glfwGetTime();
        printf("FPS: %f\r", 1 / (frameTimeEnd - frameTimeStart));
    }
}

static void cleanup(App* app)
{
    clean_vk(app);
    glfwDestroyWindow(app->window);
}

int main()
{
    App app = {
        .applicationName = "hello_world",
        .engineName = "AZEA ENGINE",
        .windowTitle = "hello",
        .vkApiVersion = VK_API_VERSION_1_4,
        .backgroundColor = { { { 0.01f, 0.1f, 0.1f, 0.01f } } },
        .maxFramesInFlight = 3,
        .start_time = current_time(),
    };

    init(&app);
    loop(&app);
    cleanup(&app);

    return EXIT_SUCCESS;
}
