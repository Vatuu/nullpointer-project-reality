#include "memory.h"
#include "malloc.h"
#include "types.h"

u8* actor_start, actor_current, actor_end;

void init_actor_memory() {
    actor_start = actor_current = malloc(ACTOR_MEMORY_SIZE);
    actor_end = actor_start + ACTOR_MEMORY_SIZE;
}