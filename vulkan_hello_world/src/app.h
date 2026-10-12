#pragma once

#include "input/input.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "argv_option.h"
#include "utils/utils.h"
#include "vk/vk_renderer.h"
#include "voxel/mesh/mesher.h"
#include "voxel/player/player.h"
#include "voxel/world/world.h"

typedef struct App
{
    const char* applicationName;
    const char* engineName;
    const char* windowTitle;
    u32 vkApiVersion;
    VkClearValue backgroundColor;
    u32 maxFramesInFlight;

    // extent of the configurable field

    struct argv_option argv_options;

    Input inputState;
    double start_time;

    // GLFW
    GLFWmonitor* windowMonitor;
    GLFWwindow* window;

    Player player;

    Renderer renderer;

    World world;
    Mesher mesher;
} App;
