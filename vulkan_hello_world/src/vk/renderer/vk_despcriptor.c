#include "vk_despcriptor.h"

#include <stddef.h>
#include <stdlib.h>

void create_descriptor_set_layout(App* app)
{
    VkDescriptorSetLayoutBinding samplerLayoutBinding = {
        .binding = 1,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImmutableSamplers = nullptr,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
    };

    VkDescriptorSetLayoutBinding bindings[1] = {samplerLayoutBinding};
    VkDescriptorSetLayoutCreateInfo layoutInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = COUNTOF(bindings),
        .pBindings = bindings,
    };

    ASSERTVK(vkCreateDescriptorSetLayout(app->renderer.context.device, &layoutInfo,
                                         app->renderer.allocator,
                                         &app->renderer.pipeline.descriptorSetLayout),
             "failed to create descriptor set layout!")
}

void destroy_descriptor_set_layout(App* app)
{
    vkDestroyDescriptorSetLayout(app->renderer.context.device,
                                 app->renderer.pipeline.descriptorSetLayout,
                                 app->renderer.allocator);
}

void create_descriptor_pool(App* app)
{
    VkDescriptorPoolSize poolSize[1] = {
        [0] =
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
    ASSERTVK(vkCreateDescriptorPool(app->renderer.context.device, &poolInfo, nullptr,
                                    &app->renderer.descriptors.pool),
             "failed to create descriptor pool!");
}

void create_descriptor_sets(App* app)
{
    VkDescriptorSetLayout* layouts = calloc(app->maxFramesInFlight, sizeof(*layouts));
    for (size_t i = 0; i < app->maxFramesInFlight; i++)
        layouts[i] = app->renderer.pipeline.descriptorSetLayout;

    VkDescriptorSetAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = app->renderer.descriptors.pool,
        .descriptorSetCount = app->maxFramesInFlight,
        .pSetLayouts = layouts,
    };

    app->renderer.descriptors.sets =
        calloc(app->maxFramesInFlight, sizeof(*app->renderer.descriptors.sets));

    ASSERTVK(vkAllocateDescriptorSets(app->renderer.context.device, &allocInfo,
                                      app->renderer.descriptors.sets),
             "failed to allocate descriptor sets!");

    for (size_t i = 0; i < app->maxFramesInFlight; i++)
    {
        VkDescriptorImageInfo imageInfo = {
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .imageView = app->renderer.texture.image.view,
            .sampler = app->renderer.texture.sampler,
        };

        VkWriteDescriptorSet descriptorWrite[1] = {
            [0] =
                {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .dstSet = app->renderer.descriptors.sets[i],
                    .dstBinding = 1,
                    .dstArrayElement = 0,
                    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    .descriptorCount = 1,
                    .pImageInfo = &imageInfo,
                },
        };
        vkUpdateDescriptorSets(app->renderer.context.device, COUNTOF(descriptorWrite),
                               descriptorWrite, 0, nullptr);
    }

    // TODO maybe we dont deallocate now ?
    free(layouts);
}

void destroy_descriptor_pool(App* app)
{
    vkDestroyDescriptorPool(app->renderer.context.device, app->renderer.descriptors.pool,
                            app->renderer.allocator);
    free(app->renderer.descriptors.sets);
}
