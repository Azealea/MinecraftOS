#include "buffer.h"

#include "vk/renderer/vk_command.h"

void copyBuffer(App* app, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBuffer commandBuffer = begin_single_time_commands(app);
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1,
                    &(VkBufferCopy){.size = size});
    end_single_time_commands(app, commandBuffer);
}

void copyBufferAt(App* app, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size,
                  VkDeviceSize dstOffset)
{
    VkCommandBuffer commandBuffer = begin_single_time_commands(app);
    vkCmdCopyBuffer(
        commandBuffer, srcBuffer, dstBuffer, 1,
        &(VkBufferCopy){.srcOffset = 0, .dstOffset = dstOffset, .size = size});
    end_single_time_commands(app, commandBuffer);
}
