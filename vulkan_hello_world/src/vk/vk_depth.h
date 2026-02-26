#pragma once

#include "app.h"

void create_depth_resources(App* app);
void destroy_depth_resources(App* app);

VkFormat findDepthFormat(App* app);
