#pragma once

#include "app.h"

void create_swapchain(App* app);
void destroy_swapchain(App* app);

uint32_t acquire_swapchain_image(App* app, uint32_t frameIndex);
void present_swapchain_image(App* app, uint32_t imageIndex);
