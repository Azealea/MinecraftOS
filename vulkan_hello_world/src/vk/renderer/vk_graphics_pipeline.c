#include "vk/renderer/vk_graphics_pipeline.h"

#include <stdlib.h>

#include "vk/buffer/vertex.h"
#include "vk/renderer/vk_shaders.h"
#include "vk/vk_depth.h"

void create_renderpass(App* app)
{
    VkAttachmentDescription colorAttachment = {
        .format = app->swapchain.format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

    VkAttachmentDescription depthAttachment = {
        .format = findDepthFormat(app),
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };

    VkAttachmentReference colorAttachmentRef = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    VkAttachmentReference depthAttachmentRef = {
        .attachment = 1,
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    VkSubpassDescription subpass = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentRef,
        .pDepthStencilAttachment = &depthAttachmentRef,
    };

    VkSubpassDependency dependency = {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
            | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
        .srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
            | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
            | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
    };

    VkAttachmentDescription attachments[] = { colorAttachment,
                                              depthAttachment };
    VkRenderPassCreateInfo renderPassInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = sizeof(attachments) / sizeof(*attachments),
        .pAttachments = attachments,
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 1,
        .pDependencies = &dependency,
    };

    ASSERTVK(vkCreateRenderPass(app->context.device, &renderPassInfo,
                                app->allocator, &app->renderer.renderpass),
             "failed to create render pass!");
}

void create_graphics_pipeline(App* app)
{
    VkPipelineShaderStageCreateInfo shaderStages[] = {
        load_shader_module(app, src_shaders_shader_vert_spv,
                           src_shaders_shader_vert_spv_len,
                           VK_SHADER_STAGE_VERTEX_BIT),

        load_shader_module(app, src_shaders_shader_frag_spv,
                           src_shaders_shader_frag_spv_len,
                           VK_SHADER_STAGE_FRAGMENT_BIT),
    };

    VkViewport viewports[] = { {
        .width = (float)app->swapchain.imageExtent.width,
        .height = (float)app->swapchain.imageExtent.height,
        .maxDepth = 1.0f,
    } };

    VkRect2D scissors[] = { {
        .extent = app->swapchain.imageExtent,
    } };

    VkPipelineColorBlendAttachmentState colorBlendAttachmentStates[] = { {
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
            | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    } };

    ASSERTVK(vkCreatePipelineLayout(
                 app->context.device,
                 &(VkPipelineLayoutCreateInfo){
                     .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                     .setLayoutCount = 1,
                     .pSetLayouts = &app->renderer.descriptorSetLayout,
                 },
                 app->allocator, &app->renderer.pipelineLayout),
             "Couldn't create pipeline layout");

    auto binding_descr = get_binding_description();
    int attribute_descr_size;
    auto attribute_descr = get_attribute_descriptions(&attribute_descr_size);

    VkResult res = vkCreateGraphicsPipelines(
        app->context.device, NULL, 1,
        &(VkGraphicsPipelineCreateInfo){
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pStages = (const VkPipelineShaderStageCreateInfo*)&shaderStages,
            .stageCount = sizeof(shaderStages) / sizeof(*shaderStages),
            .pVertexInputState =
                &(VkPipelineVertexInputStateCreateInfo){
                    .sType =
                        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                    .vertexBindingDescriptionCount = 1,
                    .pVertexBindingDescriptions = &binding_descr,
                    .vertexAttributeDescriptionCount = attribute_descr_size,
                    .pVertexAttributeDescriptions = attribute_descr },
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
                    .sType =
                        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                    .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
                },
            .pDepthStencilState =
                &(VkPipelineDepthStencilStateCreateInfo){
                    .sType =
                        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
                    .depthTestEnable = VK_TRUE,
                    .depthWriteEnable = VK_TRUE,
                    .depthCompareOp = VK_COMPARE_OP_LESS,
                    .depthBoundsTestEnable = VK_FALSE,
                    .stencilTestEnable = VK_FALSE,
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
            .renderPass = app->renderer.renderpass,
        },
        app->allocator, &app->renderer.graphicsPipeline);
    ASSERTVK(res, "Couldn't create graphics pipeline");

    for (uint32_t i = 0; i < sizeof(shaderStages) / sizeof(shaderStages[0]);
         i++)
    {
        vkDestroyShaderModule(app->context.device, shaderStages[i].module,
                              app->allocator);
    }
}

void create_framebuffers(App* app)
{
    uint32_t framebufferCount = app->swapchain.imageCount;
    app->renderer.framebuffers =
        malloc(framebufferCount * sizeof(VkFramebuffer));
    ASSERT(app->renderer.framebuffers != nullptr,
           "Couldn't allocate memory for framebuffers array");

    for (uint32_t i = 0; i < framebufferCount; ++i)
    {
        VkImageView attachments[] = { app->swapchain.imageViews[i],
                                      app->depthImageView };

        VkFramebufferCreateInfo framebufferInfo = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = app->renderer.renderpass,
            .attachmentCount = sizeof(attachments) / sizeof(*attachments),
            .pAttachments = attachments,
            .width = app->swapchain.imageExtent.width,
            .height = app->swapchain.imageExtent.height,
            .layers = 1,
        };
        ASSERTVK(vkCreateFramebuffer(app->context.device, &framebufferInfo,
                                     app->allocator,
                                     &app->renderer.framebuffers[i]),
                 "Couldn't create framebuffer %i", i);
    }
}

void destroy_framebuffers(App* app)
{
    uint32_t framebuffer_count = app->swapchain.imageCount;

    for (uint32_t framebuffer_index = 0; framebuffer_index < framebuffer_count;
         ++framebuffer_index)
    {
        vkDestroyFramebuffer(app->context.device,
                             app->renderer.framebuffers[framebuffer_index],
                             app->allocator);
    }

    free(app->renderer.framebuffers);
}

void destroy_graphics_pipeline(App* app)
{
    vkDestroyPipelineLayout(app->context.device, app->renderer.pipelineLayout,
                            app->allocator);
    vkDestroyPipeline(app->context.device, app->renderer.graphicsPipeline,
                      app->allocator);
}

void destroy_renderpass(App* app)
{
    vkDestroyRenderPass(app->context.device, app->renderer.renderpass,
                        app->allocator);
}
