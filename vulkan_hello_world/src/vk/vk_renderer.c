#include "vk_renderer.h"

#include "vk_shaders.h"

static VkShaderModule load_shader_module(const App* app,
                                         const unsigned char* data, size_t size)
{
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
    return shader_module;
}

void create_renderer(App* app)
{
    VkShaderModule vertexShaderModule = load_shader_module(
        app, src_shaders_shader_vert_spv, src_shaders_shader_vert_spv_len);

    VkShaderModule fragmentShaderModule = load_shader_module(
        app, src_shaders_shader_frag_spv, src_shaders_shader_frag_spv_len);

    VkPipelineShaderStageCreateInfo shaderStages[] = {
        (VkPipelineShaderStageCreateInfo){
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .module = vertexShaderModule,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .pName = shaderEntryFunctionName,
        },
        (VkPipelineShaderStageCreateInfo){
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .module = fragmentShaderModule,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pName = shaderEntryFunctionName,
        },
    };

    VkExtent2D imageExtent = app->swapchain.imageExtent;

    VkViewport viewports[] = { {
        .width = (float)imageExtent.width,
        .height = (float)imageExtent.height,
        .maxDepth = 1.0f,
    } };

    VkRect2D scissors[] = { {
        .extent = imageExtent,
    } };

    VkPipelineColorBlendAttachmentState colorBlendAttachmentStates[] = { {
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
            | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    } };

    ASSERT(vkCreatePipelineLayout(
               app->context.device,
               &(VkPipelineLayoutCreateInfo){
                   .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
               },
               app->allocator, &app->renderer.pipelineLayout)
               == VK_SUCCESS,
           "Couldn't create pipeline layout");

    ASSERT(
        vkCreateGraphicsPipelines(
            app->context.device, NULL, 1,
            &(VkGraphicsPipelineCreateInfo){
                .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                .pStages =
                    (const VkPipelineShaderStageCreateInfo*)&shaderStages,
                .stageCount = sizeof(shaderStages) / sizeof(*shaderStages),
                .pVertexInputState =
                    &(VkPipelineVertexInputStateCreateInfo){
                        .sType =
                            VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                    },
                .pInputAssemblyState =
                    &(VkPipelineInputAssemblyStateCreateInfo){
                        .sType =
                            VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                    },
                .pViewportState =
                    &(VkPipelineViewportStateCreateInfo){
                        .sType =
                            VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                        .viewportCount = sizeof(viewports) / sizeof(*viewports),
                        .pViewports = viewports,
                        .scissorCount = sizeof(scissors) / sizeof(*scissors),
                        .pScissors = scissors,
                    },
                .pRasterizationState =
                    &(VkPipelineRasterizationStateCreateInfo){
                        .sType =
                            VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
                        .lineWidth = 1.0,
                        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
                        .cullMode = VK_CULL_MODE_BACK_BIT,
                        .polygonMode =
                            VK_POLYGON_MODE_FILL, // TODO CHEK THAT FIXME
                    },
                .pMultisampleState =
                    &(VkPipelineMultisampleStateCreateInfo){
                        .sType =
                            VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
                    },
                .pColorBlendState =
                    &(VkPipelineColorBlendStateCreateInfo){
                        .sType =
                            VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                        .attachmentCount = sizeof(colorBlendAttachmentStates)
                            / sizeof(*colorBlendAttachmentStates),
                        .pAttachments = colorBlendAttachmentStates,
                    },
                .layout = app->renderer.pipelineLayout,
            },
            app->allocator, &app->renderer.graphicsPipeline)
            == VK_SUCCESS,
        "Couldn't create graphics pipeline");

    vkDestroyShaderModule(app->context.device, vertexShaderModule,
                          app->allocator);
    vkDestroyShaderModule(app->context.device, fragmentShaderModule,
                          app->allocator);
}

void destroy_renderer(App* app)
{
    vkDestroyPipeline(app->context.device, app->renderer.graphicsPipeline,
                      app->allocator);
    vkDestroyPipelineLayout(app->context.device, app->renderer.pipelineLayout,
                            app->allocator);
}