#pragma once

#include "app.h"

void create_texture_stuff(App* app);
void destroy_texture_stuff(App* app);

void create_image(App* app, uint32_t width, uint32_t height, VkFormat format,
                  VkImageTiling tiling, VkImageUsageFlags usage,
                  VkMemoryPropertyFlags properties, VkImage* image,
                  VkDeviceMemory* imageMemory);
