#pragma once

#include "app.h"

void create_descriptor_set_layout(App* app);
void destroy_descriptor_set_layout(App* app);

void create_descriptor_pool(App* app);
void create_descriptor_sets(App* app);

void destroy_descriptor_pool(App* app);
