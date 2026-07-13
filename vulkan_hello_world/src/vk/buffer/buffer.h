#pragma once
#include "app.h"

void copyBuffer(App* app, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
void copyBufferAt(App* app, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size,
                  VkDeviceSize dstOffset);
