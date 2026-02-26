#pragma once

#include "app.h"

void create_command_pool(App* app);
void destroy_command_pool(App* app);
void allocate_command_buffer(App* app);
void record_command_buffer(App* app, uint32_t imageIndex, uint32_t frameIndex);
void submit_command_buffer(App* app, uint32_t imageIndex, uint32_t frameIndex);

VkCommandBuffer begin_single_time_commands(App* app);
void end_single_time_commands(App* app, VkCommandBuffer commandBuffer);
