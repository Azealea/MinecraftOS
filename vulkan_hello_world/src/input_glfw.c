#include <GLFW/glfw3.h>
#include <string.h>

#include "input.h"

void input_poll(Input* state, void* backend_ctx)
{
    static const int glfw_key_map[KEY_COUNT] = {
        [KEY_W] = GLFW_KEY_W,         [KEY_S] = GLFW_KEY_S,
        [KEY_A] = GLFW_KEY_A,         [KEY_D] = GLFW_KEY_D,
        [KEY_SPACE] = GLFW_KEY_SPACE, [KEY_LEFT_SHIFT] = GLFW_KEY_LEFT_SHIFT,
        [KEY_TAB] = GLFW_KEY_TAB,
    };
    GLFWwindow* window = (GLFWwindow*)backend_ctx;

    state->meta = glfwGetKey(window, GLFW_KEY_LEFT_SUPER) == GLFW_PRESS ||
                  glfwGetKey(window, GLFW_KEY_RIGHT_SUPER) == GLFW_PRESS;

    memcpy(state->keys_prev, state->keys, sizeof(state->keys));
    for (int i = 0; i < KEY_COUNT; i++)
    {
        state->keys[i] = glfwGetKey(window, glfw_key_map[i]) == GLFW_PRESS;
    }

    memcpy(state->mouse_buttons_prev, state->mouse_buttons, sizeof(state->mouse_buttons));
    for (int i = 0; i < MOUSE_BUTTON_COUNT; i++)
    {
        state->mouse_buttons[i] = glfwGetMouseButton(window, i) == GLFW_PRESS;
    }

    double new_x, new_y;
    glfwGetCursorPos(window, &new_x, &new_y);
    state->mouse_dx = new_x - state->mouse_x;
    state->mouse_dy = new_y - state->mouse_y;
    state->mouse_x = new_x;
    state->mouse_y = new_y;
}
