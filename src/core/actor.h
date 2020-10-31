#ifndef _CC_ACTOR_
#define _CC_ACTOR_

#include <nusys.h>
#include "types.h"

struct actor_data {
    vec_3d position;
    rotation rot;
    f32 scale;
};

struct actor {
    const char* actor_id;
    struct actor_data* data;
    void (*actor_init)(void);
    void (*actor_update)(void);
    void (*actor_frame)(Mtx* matrix, Gfx* displayListPtr, f32 delta);
};

extern struct actor actor_the_n;

#endif