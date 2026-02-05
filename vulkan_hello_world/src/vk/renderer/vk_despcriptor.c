#include "vk_despcriptor.h"

#include <stddef.h>
#include <stdlib.h>

#include "vk/buffer/cam.h"

void create_descriptor_set_layout(App* app)
{
    VkDescriptorSetLayoutBinding uboLayoutBinding = {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    };
    VkDescriptorSetLayoutCreateInfo layoutInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = &uboLayoutBinding,
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
    VkDescriptorPoolSize poolSize = {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = app->maxFramesInFlight,
    };

    VkDescriptorPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = 1,
        .pPoolSizes = &poolSize,
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

        VkWriteDescriptorSet descriptorWrite = {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = app->descriptorSets[i],
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .pBufferInfo = &bufferInfo,
        };
        vkUpdateDescriptorSets(app->context.device, 1, &descriptorWrite, 0,
                               nullptr);
    }

    // TODO maybe we dont deallocate now ?
    free(layouts);
}

void destroy_descriptor_pool(App* app)
{
    vkDestroyDescriptorPool(app->context.device, app->descriptorPool,
                            app->allocator);
}
