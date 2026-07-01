#pragma once

#include "app.h"

void gpu_buffer_create(App* app, VkDeviceSize size, VkBufferUsageFlags usage,
                       VkMemoryPropertyFlags props, GpuBuffer* out);

void gpu_buffer_destroy(App* app, GpuBuffer* buf);

void gpu_image_create(App* app, uint32_t width, uint32_t height,
                      uint32_t layers, VkFormat format, VkImageTiling tiling,
                      VkImageUsageFlags usage, VkMemoryPropertyFlags props,
                      VkImageAspectFlags aspect, GpuImage* out);

void gpu_image_destroy(App* app, GpuImage* img);
