#pragma once
#include <stdbool.h>

typedef enum
{
    KEY_W,
    KEY_S,
    KEY_A,
    KEY_D,
    KEY_SPACE,
    KEY_LEFT_SHIFT,
    KEY_TAB,
    KEY_COUNT
} InputKey;

typedef struct
{
    bool keys[KEY_COUNT];
    bool keys_prev[KEY_COUNT]; // to know pressed & released

    double mouse_x, mouse_y;
    double mouse_dx, mouse_dy;
} Input;

void input_poll(Input* state, void* backend_ctx);

static inline bool input_key_held(const Input* s, InputKey k)
{
    return s->keys[k];
}
static inline bool input_key_pressed(const Input* s, InputKey k)
{
    return s->keys[k] && !s->keys_prev[k];
}
static inline bool input_key_released(const Input* s, InputKey k)
{
    return !s->keys[k] && s->keys_prev[k];
}
