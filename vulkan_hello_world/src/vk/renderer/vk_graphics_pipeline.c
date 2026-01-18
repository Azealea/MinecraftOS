#include "vk/renderer/vk_graphics_pipeline.h"

#include <stdlib.h>

#include "vk/renderer/vk_shaders.h"
#include "vk/vertex/vertex.h"

void create_renderpass(App* app)
{
    VkFormat image_format = app->swapchain.format;

    VkAttachmentReference color_attachment_references[] = { {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    } };

    VkSubpassDescription subpass_descriptions[] = { {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = color_attachment_references,
    } };

    VkAttachmentDescription attachment_descriptions[] = { {
        .format = image_format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
    } };

    ASSERT(
        vkCreateRenderPass(
            app->context.device,
            &(VkRenderPassCreateInfo){
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                .subpassCount = sizeof(subpass_descriptions)
                    / sizeof(*subpass_descriptions),
                .pSubpasses =
                    (const VkSubpassDescription*)&subpass_descriptions,
                .attachmentCount = sizeof(attachment_descriptions)
                    / sizeof(*attachment_descriptions),
                .pAttachments = attachment_descriptions,
                .pDependencies =
                    &(VkSubpassDependency){
                        .srcSubpass = VK_SUBPASS_EXTERNAL,
                        .dstSubpass = 0,
                        .srcAccessMask = 0,
                        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                        .srcStageMask =
                            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .dstStageMask =
                            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                    } },
            app->allocator, &app->renderer.renderpass)
            == VK_SUCCESS,
        "Couldn't create renderpass")
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

    VkPipelineLayout pipelineLayout;
    ASSERT(vkCreatePipelineLayout(
               app->context.device,
               &(VkPipelineLayoutCreateInfo){
                   .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
               },
               app->allocator, &pipelineLayout)
               == VK_SUCCESS,
           "Couldn't create pipeline layout");

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
                    .pVertexBindingDescriptions =
                        (VkVertexInputBindingDescription[]){ {
                            .binding = 0,
                            .stride = sizeof(Vertex),
                            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
                        } },
                    .vertexAttributeDescriptionCount = 2,
                    .pVertexAttributeDescriptions =
                        (VkVertexInputAttributeDescription[]){
                            {
                                .binding = 0,
                                .location = 0,
                                .format = VK_FORMAT_R32G32_SFLOAT,
                                .offset = offsetof(Vertex, pos),
                            },
                            {
                                .binding = 0,
                                .location = 1,
                                .format = VK_FORMAT_R32G32B32_SFLOAT,
                                .offset = offsetof(Vertex, color),
                            } },
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
                    .polygonMode = VK_POLYGON_MODE_FILL,
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
            .layout = pipelineLayout,
            .renderPass = app->renderer.renderpass,
        },
        app->allocator, &app->renderer.graphicsPipeline);
    ASSERT(res == VK_SUCCESS, "Couldn't create graphics pipeline");

    vkDestroyPipelineLayout(app->context.device, pipelineLayout,
                            app->allocator);

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

    for (uint32_t framebufferIndex = 0; framebufferIndex < framebufferCount;
         ++framebufferIndex)
    {
        ASSERT(vkCreateFramebuffer(
                   app->context.device,
                   &(VkFramebufferCreateInfo){
                       .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                       .layers = 1,
                       .renderPass = app->renderer.renderpass,
                       .width = app->swapchain.imageExtent.width,
                       .height = app->swapchain.imageExtent.height,
                       .attachmentCount = 1,
                       .pAttachments =
                           &app->swapchain.imageViews[framebufferIndex],
                   },
                   app->allocator,
                   &app->renderer.framebuffers[framebufferIndex])
                   == VK_SUCCESS,
               "Couldn't create framebuffer %i", framebufferIndex);
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
    vkDestroyPipeline(app->context.device, app->renderer.graphicsPipeline,
                      app->allocator);
}

void destroy_renderpass(App* app)
{
    vkDestroyRenderPass(app->context.device, app->renderer.renderpass,
                        app->allocator);
}
