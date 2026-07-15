#pragma once

#include "input.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "utils/utils.h"
#include "vk/camera.h"
#include "vk/vk_renderer.h"
#include "voxel/textures/face_texture.h"
#include "voxel/world.h"

typedef struct
{
    const char* applicationName;
    const char* engineName;
    const char* windowTitle;
    uint32_t vkApiVersion;
    VkClearValue backgroundColor;
    uint32_t maxFramesInFlight;

    // extent of the configurable field

    Input inputState;
    double start_time;

    // GLFW
    GLFWmonitor* windowMonitor;
    GLFWwindow* window;

    Camera camera;

    Renderer renderer;

    FaceTexture block_faces[BLOCK_COUNT][BLOCK_FACE_COUNT];
    World world;
} App;
