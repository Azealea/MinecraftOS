#include <stdlib.h>

#include "app.h"
#include "argv_option.h"
#include "glfw.h"
#include "input/input.h"
#include "utils/time.h"
#include "vk/vk.h"
#include "voxel/mesh/mesher.h"
#include "voxel/player/player_movement.h"
#include "voxel/world/world.h"

static void display_info()
{
    uint32_t instanceApiVersion;
    ASSERTVK(vkEnumerateInstanceVersion(&instanceApiVersion),
             "Couldn't enumerate instance version");
    uint32_t apiVersionVariant = VK_API_VERSION_VARIANT(instanceApiVersion);
    uint32_t apiVersionMajor = VK_API_VERSION_MAJOR(instanceApiVersion);
    uint32_t apiVersionMinor = VK_API_VERSION_MINOR(instanceApiVersion);
    uint32_t apiVersionPatch = VK_API_VERSION_PATCH(instanceApiVersion);
    INFO("Vulkan API %i.%i.%i.%i\n", apiVersionVariant, apiVersionMajor, apiVersionMinor,
         apiVersionPatch);
    INFO("GLFW %s\n", glfwGetVersionString());
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
    const GLFWvidmode* mode = glfwGetVideoMode(app->windowMonitor);
    const double targetFrameTime = 1.0 / mode->refreshRate;
    // const double targetFrameTime = 1.0 / 60.0;

    while (!glfwWindowShouldClose(app->window))
    {
        double frameTimeStart = glfwGetTime();
        glfwPollEvents();
        input_poll(&app->inputState, app->window);
        player_consume_input(app);
        rebuild_dirty_meshes(app);
        drawFrame(app);
        double frameTimeEnd = glfwGetTime();
        //  printf("FPS: %f\r", 1 / (frameTimeEnd - frameTimeStart));

        sleep_seconds(targetFrameTime - (frameTimeEnd - frameTimeStart));
    }
}

static void cleanup(App* app)
{
    world_clean(&app->world);
    mesher_clean(&app->mesher);

    clean_vk(app);
    glfwDestroyWindow(app->window);
    glfwTerminate();
}

static void run(App* app)
{
    ChunkPos origin = {0, 0, 0};
    generate_chunk_terrain(&app->world, origin, &app->mesher.dirty);

    init(app);
    loop(app);
    cleanup(app);
}

int main(int argc, char** argv)
{
    App app = {
        .applicationName = "hello_world",
        .engineName = "AZEA ENGINE",
        .windowTitle = "KubeOs",
        .vkApiVersion = VK_API_VERSION_1_4,
        .backgroundColor = {{{0.01f, 0.1f, 0.1f, 0.01f}}},
        .maxFramesInFlight = 3,
        .start_time = current_time(),
        .player = player_init((VEC3(i32)){0, 0, 0}),
        .world = world_init(),
        .mesher = mesher_init(),
        .argv_options = parse_options(argc, argv),
    };

    run(&app);

    return EXIT_SUCCESS;
}
