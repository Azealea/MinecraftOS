#pragma once

#include "app.h"

void create_swapchain(App* app);
void destroy_swapchain(App* app);

void acquire_swapchain_image(App* app);
void present_swapchain_image(App* app);