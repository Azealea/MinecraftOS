#include "vk_despcriptor.h"

#include <stddef.h>
#include <stdlib.h>

#include "vk/camera_ubo.h"

void create_descriptor_set_layout(App* app)
{
    VkDescriptorSetLayoutBinding uboLayoutBinding = {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    };

    VkDescriptorSetLayoutBinding samplerLayoutBinding = {
        .binding = 1,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImmutableSamplers = nullptr,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
    };

    VkDescriptorSetLayoutBinding bindings[2] = {uboLayoutBinding,
                                                samplerLayoutBinding};
    VkDescriptorSetLayoutCreateInfo layoutInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = COUNTOF(bindings),
        .pBindings = bindings,
    };

    ASSERTVK(vkCreateDescriptorSetLayout(app->context.device, &layoutInfo,
                                         app->allocator,
                                         &app->renderer.descriptorSetLayout),
             "failed to create descriptor set layout!")
}

void destroy_descriptor_set_layout(App* app)
{
    vkDestroyDescriptorSetLayout(
        app->context.device, app->renderer.descriptorSetLayout, app->allocator);
}

void create_descriptor_pool(App* app)
{
    VkDescriptorPoolSize poolSize[2] = {
        [0] =
            {
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = app->maxFramesInFlight,
            },

        [1] =
            {
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = app->maxFramesInFlight,
            },
    };

    VkDescriptorPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = COUNTOF(poolSize),
        .pPoolSizes = poolSize,
        .maxSets = app->maxFramesInFlight,
    };
    ASSERTVK(vkCreateDescriptorPool(app->context.device, &poolInfo, nullptr,
                                    &app->descriptorPool),
             "failed to create descriptor pool!");
}

void create_descriptor_sets(App* app)
{
    VkDescriptorSetLayout* layouts =
        malloc(sizeof(*layouts) * app->maxFramesInFlight);
    for (size_t i = 0; i < app->maxFramesInFlight; i++)
        layouts[i] = app->renderer.descriptorSetLayout;

    VkDescriptorSetAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = app->descriptorPool,
        .descriptorSetCount = app->maxFramesInFlight,
        .pSetLayouts = layouts,
    };

    app->descriptorSets =
        calloc(app->maxFramesInFlight, sizeof(*app->descriptorSets));

    ASSERTVK(vkAllocateDescriptorSets(app->context.device, &allocInfo,
                                      app->descriptorSets),
             "failed to allocate descriptor sets!");

    for (size_t i = 0; i < app->maxFramesInFlight; i++)
    {
        VkDescriptorBufferInfo bufferInfo = {
            .buffer = app->uniformBuffers[i],
            .offset = 0,
            .range = sizeof(struct UniformBufferObject),
        };
        VkDescriptorImageInfo imageInfo = {
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .imageView = app->textureImageView,
            .sampler = app->textureSampler,
        };

        VkWriteDescriptorSet descriptorWrite[2] = {
            [0] =
                {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .dstSet = app->descriptorSets[i],
                    .dstBinding = 0,
                    .dstArrayElement = 0,
                    .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    .descriptorCount = 1,
                    .pBufferInfo = &bufferInfo,
                },
            [1] =
                {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .dstSet = app->descriptorSets[i],
                    .dstBinding = 1,
                    .dstArrayElement = 0,
                    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    .descriptorCount = 1,
                    .pImageInfo = &imageInfo,
                },
        };
        vkUpdateDescriptorSets(app->context.device, COUNTOF(descriptorWrite),
                               descriptorWrite, 0, nullptr);
    }

    // TODO maybe we dont deallocate now ?
    free(layouts);
}

void destroy_descriptor_pool(App* app)
{
    vkDestroyDescriptorPool(app->context.device, app->descriptorPool,
                            app->allocator);
}
