#pragma once
#include "app.h"

void create_buffer(App* app, VkDeviceSize size, VkBufferUsageFlags usage,
                   VkMemoryPropertyFlags properties, VkBuffer* buffer,
                   VkDeviceMemory* bufferMemory);

void copyBuffer(App* app, VkBuffer srcBuffer, VkBuffer dstBuffer,
                VkDeviceSize size);
