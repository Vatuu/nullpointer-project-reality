#ifndef _CC_ACTOR_
#define _CC_ACTOR_

#include <nusys.h>
#include "types.h"
#include "gfx_management.h"

#define physical(x) x->data_physical
#define collidable(x) x->data_physical->data_collidable

#define type_physical(x) x->actor_type->actor_physical

// Actor Data Structs

typedef struct actor_data {
    struct actor* actor_type;
    bool enabled;

    struct actor_data_physical* data_physical;
} actor_data;

typedef struct actor_data_physical {
    vec_3d position;
    rotation_3d rotation;
    f32 scale;
    bool drawing_enabled;

    struct actor_data_collidable* data_collidable;
} actor_data_physical;

typedef struct actor_data_collidable {
    vec_3d hitbox_0, hitbox_2;
    vec_3d velocity;
    bool collision_enabled, on_ground;

    //struct actor_data_playable* data_playable;
} actor_data_collidable;

/*typedef struct actor_data_playable {

};*/

// Actor Definiton Structs

typedef struct actor {
    const char* actor_id;
    void (*actor_init)(actor_data* data);
    void (*actor_update)(actor_data* data);

    struct physical_actor* actor_physical;
} actor;

typedef struct physical_actor {
    void (*actor_frame)(actor_data* data, f32 delta);

    struct collidable_actor* actor_collidable;
} physical_actor;

typedef struct collidable_actor {
    bool (*actor_is_colliding)(actor_data* data);
    void (*actor_on_collide)(actor_data* data);

    //struct playable_actor* actor_playable;
} collidable_actor;

/*typedef struct playable_actor {

} playable_actor;*/

extern size_t actor_physical_create(actor_data* actors, actor* type, bool enabled, vec_3d position, rotation_3d rot, f32 scale, bool drawing);

extern struct actor actor_the_n;

#endif