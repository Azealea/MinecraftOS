#include <stdlib.h>

#include "app.h"
#include "argv_option.h"
#include "glfw.h"
#include "input/input.h"
#include "utils/container.h"
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
    INFO("Vulkan API %i.%i.%i.%i", VK_API_VERSION_VARIANT(instanceApiVersion),
         VK_API_VERSION_MAJOR(instanceApiVersion),
         VK_API_VERSION_MINOR(instanceApiVersion),
         VK_API_VERSION_PATCH(instanceApiVersion));
    INFO("GLFW %s", glfwGetVersionString());
}

static void init(App* app)
{
    MAP_FOR_EACH(app->world.chunks, chunk)
    {
        world_mark_chunk_dirty(&app->world, chunk->key, &app->mesher.dirty);
    }

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
    world_clean(&app->world, app->argv_options.save_to);
    mesher_clean(&app->mesher);

    clean_vk(app);
    glfwDestroyWindow(app->window);
    glfwTerminate();
}

static void run(App* app)
{
    init(app);
    loop(app);
    cleanup(app);
}

int main(int argc, char** argv)
{
    struct argv_option options = parse_options(argc, argv);
    App app = {
        .applicationName = "hello_world",
        .engineName = "AZEA ENGINE",
        .vkApiVersion = VK_API_VERSION_1_4,
        .backgroundColor = {{{0.01f, 0.1f, 0.1f, 0.01f}}},
        .maxFramesInFlight = 3,
        .start_time = current_time(),
        .player = player_init((VEC3(i32)){8, 8, 2}),
        .world = world_init(options.load_from),
        .mesher = mesher_init(),
        .argv_options = options,
    };

    run(&app);

    return EXIT_SUCCESS;
}
