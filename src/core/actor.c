#include <malloc.h>

#include "actor.h"
#include "string.h"
#include "debug.h"

size_t actor_physical_create(actor_data* actors, actor* type, bool enabled, vec_3d position, rotation_3d rot, f32 scale, bool drawing) {
    size_t i = 0;
    for(; i < MAX_ACTORS; i++)
        if(actors[i].actor_type == NULL)
            break;

    actor_data* actor = &actors[i];
    actor->actor_type = type;
    actor->enabled = enabled;
    actor->data_physical = malloc(sizeof(actor_data_physical));
    physical(actor)->position = position;
    physical(actor)->rotation = rot;
    physical(actor)->scale = scale;
    physical(actor)->drawing_enabled = drawing;

    type->actor_init(actor);

    return i;
}