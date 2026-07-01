#include "buffer.h"

#include "vk/renderer/vk_command.h"

void copyBuffer(App* app, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBuffer commandBuffer = begin_single_time_commands(app);

    VkBufferCopy copyRegion = {
        .size = size,
    };

    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    end_single_time_commands(app, commandBuffer);
}
