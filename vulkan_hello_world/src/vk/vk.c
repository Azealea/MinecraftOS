#include "vk/vk.h"

#include "vk/renderer/vk_command.h"
#include "vk/renderer/vk_renderer.h"
#include "vk/vk_context.h"
#include "vk/vk_swapchain.h"

void init_vk(App* app)
{
    create_vk_context(app);
    create_swapchain(app);
    create_renderer(app);
}

void clean_vk(App* app)
{
    destroy_renderer(app);
    destroy_swapchain(app);
    destroy_vk_context(app);
}

void do_stuff_vk(App* app)
{
    acquire_swapchain_image(app);
    record_command_buffer(app);
    submit_command_buffer(app);
    present_swapchain_image(app);
}