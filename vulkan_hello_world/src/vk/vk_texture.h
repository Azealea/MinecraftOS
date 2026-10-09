#pragma once

#include "app.h"
#include "voxel/textures/builder/array_atlas.h"

void create_texture_image_from_atlas(App* app, VECTOR(texture_t)* atlas);
void create_texture_sampler(App* app);
void destroy_texture_sampler(App* app);
void destroy_texture_stuff(App* app);
