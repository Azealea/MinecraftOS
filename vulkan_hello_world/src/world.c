#include "world.h"

#include "block.h"
#include "chunk.h"

World world_constr(void)
{
    World w = {0};
    w.chunk = chunk_constr(0, 0);

    for (int i = 0; i < 4; i++)
    {
        chunk_set(w.chunk, i * 2, 0, 0, (Block){.type = i});
    }

    return w;
}

void world_free(World* w)
{
    chunk_deconstr(w->chunk);
}
