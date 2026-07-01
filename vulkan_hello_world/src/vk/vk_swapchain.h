#pragma once

#include "app.h"

void create_swapchain(App* app);
void destroy_swapchain(App* app);
VkImageView create_image_view(App* app, VkImage image, VkFormat format,
                              VkImageAspectFlags aspectFlags, uint32_t layers);

uint32_t acquire_swapchain_image(App* app, uint32_t frameIndex);
void present_swapchain_image(App* app, uint32_t imageIndex);
