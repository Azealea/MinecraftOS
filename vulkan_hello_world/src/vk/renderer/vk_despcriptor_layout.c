#include "vk_despcriptor_layout.h"

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
