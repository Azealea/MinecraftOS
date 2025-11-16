#include <stdlib.h>

#include "app.h"

void glfw_error_callback(int errorCode, const char* description)
{
    ASSERT(errorCode == 0, "GLFW: %s", description);
}

void exit_callback()
{
    glfwTerminate();
}

static void setup_error_handling()
{
    glfwSetErrorCallback(glfw_error_callback);
    atexit(exit_callback);
}

void init_glfw(App* app)
{
    setup_error_handling();
    ASSERT(glfwInit() == GLFW_TRUE, "glfwInit failed");
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, app->windowResizable);
}

void create_glfw_window(App* app)
{
    if (app->windowFullscreen)
    {
        app->windowMonitor = glfwGetPrimaryMonitor();

        const GLFWvidmode* mode = glfwGetVideoMode(app->windowMonitor);
        app->windowWidth = mode->width;
        app->windowHeight = mode->height;
    }

    app->window = glfwCreateWindow(app->windowWidth, app->windowHeight,
                                   app->windowTitle, app->windowMonitor, NULL);

    ASSERT(app->window, "glfwCreateWindow failed");
}
