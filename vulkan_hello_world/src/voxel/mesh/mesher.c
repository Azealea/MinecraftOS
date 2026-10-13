#include "mesher.h"

#include "app.h"
#include "vk/buffer/vertex.h"
#include "voxel/mesh/mesh.h"

Mesher mesher_init(void)
{
    return (Mesher){
        .dirty = {0},
        .mesh = bucket_alloc_init(),
    };
}

void mesher_clean(Mesher* m)
{
    bucket_alloc_clean(&m->mesh);
    VECTOR_FREE(&m->dirty);
}

void mesher_mesh_chunk(Mesher* m, World* w, ChunkPos pos)
{
    Chunk* c = world_get_chunk(w, pos);
    ASSERT(c != NULL, "mesher_mesh_chunk: chunk " VEC3_FMT " does not exist",
           VEC3_ARGS(pos));
    if (c->bucket_ptr == NULL)
        c->bucket_ptr = bucket_alloc_acquire(&m->mesh);
    c->face_count = generate_chunk_mesh(c, pos, w, m->Block2TextureMap);
}

void rebuild_dirty_meshes(App* app)
{
    World* w = &app->world;
    Mesher* m = &app->mesher;
    VECTOR_FOR_EACH(&m->dirty, pos)
    {
        mesher_mesh_chunk(m, w, *pos);
        Chunk* c = world_get_chunk(w, *pos);
        upload_chunk_mesh(app, c->bucket_ptr, c->face_count);
        c->dirty = false;
    }
    VECTOR_CLEAR(&m->dirty);
}
