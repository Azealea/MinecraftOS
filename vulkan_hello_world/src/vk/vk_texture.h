#pragma once

#include "app.h"

void create_texture_image(App* app);
void destroy_texture_image(App* app);

void create_texture_image_view(App* app);
void destroy_texture_image_view(App* app);

void create_texture_sampler(App* app);
void destroy_texture_sampler(App* app);

void create_image(App* app, uint32_t width, uint32_t height, VkFormat format,
                  VkImageTiling tiling, VkImageUsageFlags usage,
                  VkMemoryPropertyFlags properties, VkImage* image,
                  VkDeviceMemory* imageMemory);
void transition_image_layout(App* app, VkImage image, VkFormat format,
                             VkImageLayout oldLayout, VkImageLayout newLayout);
