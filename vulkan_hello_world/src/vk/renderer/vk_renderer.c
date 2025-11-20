#include "vk/renderer/vk_renderer.h"

#include "vk/renderer/vk_command.h"
#include "vk/renderer/vk_graphics_pipeline.h"
#include "vk/renderer/vk_sync_objects.h"

void create_renderer(App* app)
{
    create_renderpass(app);
    create_graphics_pipeline(app);
    create_framebuffers(app);

    create_command_pool(app);
    allocate_command_buffer(app);
    create_sync_objects(app);
}

void destroy_renderer(App* app)
{
    vkQueueWaitIdle(app->context.queue);

    destroy_sync_objects(app);
    destroy_command_pool(app);

    destroy_framebuffers(app);
    destroy_graphics_pipeline(app);
    destroy_renderpass(app);
}