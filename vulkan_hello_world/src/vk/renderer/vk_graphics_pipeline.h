#pragma once

#include "app.h"

void create_renderpass(App* app);
void create_graphics_pipeline(App* app);
void create_framebuffers(App* app);
void destroy_framebuffers(App* app);
void destroy_graphics_pipeline(App* app);
void destroy_renderpass(App* app);