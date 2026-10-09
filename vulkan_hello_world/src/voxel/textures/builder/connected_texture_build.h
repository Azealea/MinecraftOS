#pragma once

#include <stdint.h>

#include "voxel/textures/builder/array_atlas.h"

void bake_connected4(VECTOR(texture_t)* atlas, texture_t inner_px, texture_t border_px);
void bake_connected8(VECTOR(texture_t)* atlas, texture_t inner_px, texture_t border_px);
