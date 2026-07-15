#include "raycast.h"

#include <math.h>

#include "utils/vec.h"
#include "voxel/block.h"
#include "voxel/world/chunk.h"
#include "voxel/world/world.h"

RaycastHit raycast(const Camera* cam, World* world, float max_dist)
{
    VEC3(f32)
    o = {
        cam->pos[0],
        cam->pos[1],
        cam->pos[2],
    };
    VEC3(f32)
    d = {
        cam->basis.front[0],
        cam->basis.front[1],
        cam->basis.front[2],
    };

    VEC3(i32) b = {floorf(o.x), floorf(o.y), floorf(o.z)};
    VEC3(i32) s = {d.x > 0 ? 1 : -1, d.y > 0 ? 1 : -1, d.z > 0 ? 1 : -1};

    VEC3(f32)
    t = {
        d.x != 0 ? ((s.x > 0 ? (b.x + 1.0f) : b.x) - o.x) / d.x : 1e30f,
        d.y != 0 ? ((s.y > 0 ? (b.y + 1.0f) : b.y) - o.y) / d.y : 1e30f,
        d.z != 0 ? ((s.z > 0 ? (b.z + 1.0f) : b.z) - o.z) / d.z : 1e30f,
    };
    VEC3(f32)
    dt = {
        d.x != 0 ? fabsf(1.0f / d.x) : 1e30f,
        d.y != 0 ? fabsf(1.0f / d.y) : 1e30f,
        d.z != 0 ? fabsf(1.0f / d.z) : 1e30f,
    };

    VEC3(i32) n = {0};

    for (int i = 0; i < (int)(max_dist / 0.01f); i++)
    {
        if (b.x >= 0 && b.y >= 0 && b.z >= 0)
        {
            ChunkPos cp = VEC3_DIV(b, CHUNK_SIZE);

            const Chunk* chunk = world_get_chunk(world, cp);
            if (!chunk)
                break;

            VEC3(u8) loc = pos_glob_to_rel(b);

            const Block* blk = chunk_get(chunk, loc);
            if (blk->type != BLK_AIR)
            {
                return ((RaycastHit){.hit = true, .block = b, .normal = n});
            }
        }

        if (t.x < t.y && t.x < t.z)
        {
            b.x += s.x;
            n = (VEC3(i32)){-s.x, 0, 0};
            t.x += dt.x;
        }
        else if (t.y < t.z)
        {
            b.y += s.y;
            n = (VEC3(i32)){0, -s.y, 0};
            t.y += dt.y;
        }
        else
        {
            b.z += s.z;
            n = (VEC3(i32)){0, 0, -s.z};
            t.z += dt.z;
        }

        if (fminf(t.x - dt.x, fminf(t.y - dt.y, t.z - dt.z)) > max_dist)
            break;
    }

    return ((RaycastHit){.hit = false});
}
