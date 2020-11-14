#include "actor.h"
#include "string.h"

size_t actor_create(actor_data* actors, actor* type, vec_3d position, rotation_3d rot, f32 scale, bool hidden) {
    size_t i = 0;
    for(; i < MAX_ACTORS; i++)
        if(actors[i].actor_type == NULL)
            break;

    actor_data* actor = &actors[i];
    actor->actor_type = type;
    actor->position = position;
    actor->rotation = rot;
    actor->scale = scale;
    actor->hidden = hidden;

    type->actor_init(actor);

    return i;
}