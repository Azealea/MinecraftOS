#pragma once

#include <stdint.h>

#include "voxel/textures/array_atlas.h"

void bake_connected4(ArrayAtlas* atlas, const uint8_t* inner_px,
                     const uint8_t* border_px);
void bake_connected8(ArrayAtlas* atlas, const uint8_t* inner_px,
                     const uint8_t* border_px);
