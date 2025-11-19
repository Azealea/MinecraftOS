#pragma once

#include "app.h"

extern unsigned char src_shaders_shader_vert_spv[];
extern unsigned int src_shaders_shader_vert_spv_len;
extern unsigned char src_shaders_shader_frag_spv[];
extern unsigned int src_shaders_shader_frag_spv_len;

VkShaderModule create_shader_module_from_embedded(VkDevice device,
                                                  const unsigned char* data,
                                                  size_t size);

const char* shaderEntryFunctionName = "main";