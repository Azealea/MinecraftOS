
#include "vk/renderer/vk_shaders.h"

VkPipelineShaderStageCreateInfo load_shader_module(const App* app,
                                                   const unsigned char* data,
                                                   size_t size,
                                                   VkShaderStageFlagBits stage)
{
    static const char* shaderEntryFunctionName = "main";
    VkShaderModule shader_module;

    ASSERT(vkCreateShaderModule(
               app->context.device,
               &(VkShaderModuleCreateInfo){
                   .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                   .pCode = (const uint32_t*)data,
                   .codeSize = size,
               },
               app->allocator, &shader_module)
               == VK_SUCCESS,
           "Couldn't create vertex shader module");
    return (VkPipelineShaderStageCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .module = shader_module,
        .stage = stage,
        .pName = shaderEntryFunctionName,
    };
}
