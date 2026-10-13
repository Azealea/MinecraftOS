#include "array_atlas.h"

#include "base_texture_enum.h"
#include "utils/container.h"
#include "utils/utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

texture_t load_rgba(const char* path)
{
    int image_w, image_h, chanel;
    texture_t px = stbi_load(path, &image_w, &image_h, &chanel, 4);

    ASSERT(px, "failed to load texture %s", path);
    ASSERT(image_w == TEXTURE_WIDTH_HEIGHT && image_h == TEXTURE_WIDTH_HEIGHT,
           "all textures must be same size");

    return px;
}

void atlas_push_from_base_texture(VECTOR(texture_t)* atlas, enum BaseTexture bid,
                                  uint16_t frame_count)
{
    for (int i = 0; i < frame_count; i++)
        VECTOR_PUSH_BACK(atlas, load_rgba(TexturePaths[bid + i]));
}

void atlas_push_pixels(VECTOR(texture_t)* atlas, texture_t rgba)
{
    VECTOR_PUSH_BACK(atlas, rgba);
}

void atlas_clean(VECTOR(texture_t)* atlas)
{
    VECTOR_FOR_EACH(atlas, texture)
    {
        stbi_image_free(*texture);
    }
    VECTOR_FREE(atlas);
}
