#ifndef _CC_ACTOR_
#define _CC_ACTOR_

#include <nusys.h>
#include "types.h"
#include "gfx_management.h"

typedef struct actor_data {
    struct actor* actor_type;
    vec_3d position;
    rotation_3d rotation;
    f32 scale;
    bool hidden;
} actor_data;

typedef struct actor {
    const char* actor_id;
    void (*actor_init)(actor_data* data);
    void (*actor_update)(actor_data* data);
    void (*actor_frame)(actor_data* data, f32 delta);
} actor;

extern size_t actor_create(actor_data* actors, actor* type, vec_3d position, rotation_3d rot, f32 scale, bool hidden);

extern struct actor actor_the_n;

#endif