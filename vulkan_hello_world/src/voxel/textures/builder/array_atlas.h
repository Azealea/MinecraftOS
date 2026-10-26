#pragma once

#include "base_texture_enum.h"
#include "utils/container.h"

#define TEXTURE_WIDTH_HEIGHT 16
#define MAX_TEXTURE 600

typedef uint8_t* texture_t;

VECTOR_DECLARE(texture_t);

void atlas_clean(VECTOR(texture_t)* atlas);

void atlas_push_from_base_texture(VECTOR(texture_t)* atlas, enum BaseTexture bid,
                                  uint16_t frame_count);

texture_t load_rgba(const char* path);
