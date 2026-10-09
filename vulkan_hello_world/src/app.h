#pragma once

#include "input/input.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "utils/utils.h"
#include "vk/vk_renderer.h"
#include "voxel/player/player.h"
#include "voxel/textures/resolve/face_texture.h"
#include "voxel/world/world.h"

typedef struct
{
    const char* applicationName;
    const char* engineName;
    const char* windowTitle;
    u32 vkApiVersion;
    VkClearValue backgroundColor;
    u32 maxFramesInFlight;

    // extent of the configurable field

    Input inputState;
    double start_time;

    // GLFW
    GLFWmonitor* windowMonitor;
    GLFWwindow* window;

    Player player;

    Renderer renderer;

    FaceTexture Block2TextureMap[BLOCK_COUNT][6];
    World world;
} App;
