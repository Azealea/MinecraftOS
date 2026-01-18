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

void init_glfw()
{
    setup_error_handling();
    ASSERT(glfwInit() == GLFW_TRUE, "glfwInit failed");
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

void create_glfw_window(App* app)
{
    app->windowMonitor = glfwGetPrimaryMonitor();

    const GLFWvidmode* mode = glfwGetVideoMode(app->windowMonitor);

    app->window = glfwCreateWindow(mode->width, mode->height, app->windowTitle,
                                   app->windowMonitor, NULL);

    ASSERT(app->window, "glfwCreateWindow failed");
}
