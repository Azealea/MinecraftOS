#include "texture_array_atlas.h"

#include <assert.h>

#include "base_texture_enum.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void atlas_init(TextureArrayAtlas* a)
{
    a->count = 0;
    a->pixels = malloc(sizeof(uint8_t*) * MAX_TEXTURE);
}

uint16_t atlas_get_current_slot(TextureArrayAtlas* a)
{
    return a->count;
}

uint8_t* load_rgba(const char* path)
{
    int iw, ih, ch;
    uint8_t* px = stbi_load(path, &iw, &ih, &ch, 4);
    assert(px && "failed to load texture");

    assert(iw == TEXTURE_WIDTH_HEIGHT && ih == TEXTURE_WIDTH_HEIGHT
           && "all textures must be same size");

    return px;
}

static uint32_t atlas_push(TextureArrayAtlas* a, uint8_t* rgba)
{
    assert(a->count < MAX_TEXTURE);
    a->pixels[a->count++] = rgba;
    return a->count - 1;
}

void atlas_push_from_base_texture(TextureArrayAtlas* a, BASE_TEXTURE bid,
                                  uint16_t frame_count)
{
    for (int i = 0; i < frame_count; i++)
        atlas_push(a, load_rgba(TexturePaths[bid + i]));
}

void atlas_free(TextureArrayAtlas* a)
{
    for (uint32_t i = 0; i < a->count; i++)
        stbi_image_free(a->pixels[i]);
    free(a->pixels);
}
