#include "vk/renderer/vk_graphics_pipeline.h"

#include "vk/buffer/vertex.h"
#include "vk/renderer/vk_shaders.h"
#include "vk/vk_depth.h"

void create_graphics_pipeline(App* app)
{
    VkPipelineShaderStageCreateInfo shaderStages[] = {
        load_shader_module(app, src_shaders_shader_vert_spv,
                           src_shaders_shader_vert_spv_len, VK_SHADER_STAGE_VERTEX_BIT),

        load_shader_module(app, src_shaders_shader_frag_spv,
                           src_shaders_shader_frag_spv_len, VK_SHADER_STAGE_FRAGMENT_BIT),
    };

    VkViewport viewports[] = {{
        .width = (float)app->renderer.swapchain.extent.width,
        .height = (float)app->renderer.swapchain.extent.height,
        .maxDepth = 1.0f,
    }};

    VkRect2D scissors[] = {{
        .extent = app->renderer.swapchain.extent,
    }};

    VkPipelineColorBlendAttachmentState colorBlendAttachmentStates[] = {{
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
            | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    }};

    ASSERTVK(vkCreatePipelineLayout(
                 app->renderer.context.device,
                 &(VkPipelineLayoutCreateInfo){
                     .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                     .setLayoutCount = 1,
                     .pSetLayouts = &app->renderer.pipeline.descriptorSetLayout,
                 },
                 app->renderer.allocator, &app->renderer.pipeline.layout),
             "Couldn't create pipeline layout");

    auto binding_descr = get_binding_description();
    int attribute_descr_size;
    auto attribute_descr = get_attribute_descriptions(&attribute_descr_size);

    VkResult res = vkCreateGraphicsPipelines(
        app->renderer.context.device, NULL, 1,
        &(VkGraphicsPipelineCreateInfo){
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pStages = (const VkPipelineShaderStageCreateInfo*)&shaderStages,
            .stageCount = COUNTOF(shaderStages),
            .pVertexInputState =
                &(VkPipelineVertexInputStateCreateInfo){
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                    .vertexBindingDescriptionCount = 1,
                    .pVertexBindingDescriptions = &binding_descr,
                    .vertexAttributeDescriptionCount = attribute_descr_size,
                    .pVertexAttributeDescriptions = attribute_descr},
            .pInputAssemblyState =
                &(VkPipelineInputAssemblyStateCreateInfo){
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                    .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                },
            .pViewportState =
                &(VkPipelineViewportStateCreateInfo){
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                    .viewportCount = COUNTOF(viewports),
                    .pViewports = viewports,
                    .scissorCount = COUNTOF(scissors),
                    .pScissors = scissors,
                },
            .pRasterizationState =
                &(VkPipelineRasterizationStateCreateInfo){
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
                    .depthClampEnable = VK_FALSE,
                    .rasterizerDiscardEnable = VK_FALSE,
                    .polygonMode = VK_POLYGON_MODE_FILL,
                    .lineWidth = 1.0f,
                    .cullMode = VK_CULL_MODE_BACK_BIT,
                    .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
                    .depthBiasEnable = VK_FALSE,

                },
            .pMultisampleState =
                &(VkPipelineMultisampleStateCreateInfo){
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                    .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
                },
            .pDepthStencilState =
                &(VkPipelineDepthStencilStateCreateInfo){
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
                    .depthTestEnable = VK_TRUE,
                    .depthWriteEnable = VK_TRUE,
                    .depthCompareOp = VK_COMPARE_OP_LESS,
                    .depthBoundsTestEnable = VK_FALSE,
                    .stencilTestEnable = VK_FALSE,
                },
            .pColorBlendState =
                &(VkPipelineColorBlendStateCreateInfo){
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                    .attachmentCount = COUNTOF(colorBlendAttachmentStates),
                    .pAttachments = colorBlendAttachmentStates,
                },
            .layout = app->renderer.pipeline.layout,
            .renderPass = VK_NULL_HANDLE,
            .pNext =
                &(VkPipelineRenderingCreateInfo){
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
                    .colorAttachmentCount = 1,
                    .pColorAttachmentFormats = &app->renderer.swapchain.format,
                    .depthAttachmentFormat = findDepthFormat(app),
                },
        },
        app->renderer.allocator, &app->renderer.pipeline.graphics);
    ASSERTVK(res, "Couldn't create graphics pipeline");

    for (uint32_t i = 0; i < COUNTOF(shaderStages); i++)
    {
        vkDestroyShaderModule(app->renderer.context.device, shaderStages[i].module,
                              app->renderer.allocator);
    }
}

void destroy_graphics_pipeline(App* app)
{
    vkDestroyPipelineLayout(app->renderer.context.device, app->renderer.pipeline.layout,
                            app->renderer.allocator);
    vkDestroyPipeline(app->renderer.context.device, app->renderer.pipeline.graphics,
                      app->renderer.allocator);
}
