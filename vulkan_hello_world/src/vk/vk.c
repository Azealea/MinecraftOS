#include "vk/vk.h"

#include "vk/vk_context.h"
#include "vk/vk_swapchain.h"

void init_vk(App* app)
{
    create_vk_context(app);
    create_swapchain(app);
}

void clean_vk(App* app)
{
    destroy_swapchain(app);
    destroy_vk_context(app);
}