#pragma once

#include "app.h"

void select_physical_device(App* app);
void select_queue_family(App* app);
void create_device(App* app);
void get_queue(App* app);

uint32_t find_memory_type(App* app, uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);
