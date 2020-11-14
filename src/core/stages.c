#include "stages.h"
#include "debug.h"

struct stage* current_stage = &stage00_n64;
actor_data ACTORS[MAX_ACTORS];

void change_stage(struct stage* newStage) {
    current_stage = newStage;
    current_stage->stage_init();
}

void actors_update() {
    for(size_t i = 0; i < MAX_ACTORS; i++)
        if(ACTORS[i].actor_type != NULL)
            ACTORS[i].actor_type->actor_update(&ACTORS[i]);
}

void actors_render() {
    debug_printf("  Starting Actor-Rendering\n");

    for(size_t i = 0; i < MAX_ACTORS; i++) {

        debug_printf("Index: %d | Actor: %s | Hidden: %s\n", i, ACTORS[i].actor_type == NULL ? "Empty Slot" : ACTORS[i].actor_type->actor_id, ACTORS[i].hidden);

        if(ACTORS[i].actor_type == NULL || ACTORS[i].hidden == true)
            continue;

        actor_data* cur = &ACTORS[i];

        debug_printf("    Rendering %s\n", cur->actor_type->actor_id);

        guPosition(&currentTask->objTransform[i], ROT_GET(cur->rotation), cur->scale, VEC_GET(cur->position));

        gSPMatrix(displayListPtr++, &(currentTask->objTransform[i]), G_MTX_MODELVIEW | G_MTX_PUSH | G_MTX_MUL);

        cur->actor_type->actor_frame(cur, 1.0F);

        gDPPipeSync(displayListPtr++);

        gSPPopMatrix(displayListPtr++, G_MTX_MODELVIEW);
    }

    debug_printf("  Finished Actor-Rendering\n");
}

void stage_update() {
    if(current_stage == NULL)
        change_stage(&stage00_n64);

    current_stage->stage_update();
    actors_update();
}

void stage_render() {
    debug_printf("Starting Stage-Rendering\n");

    unsigned short perspNorm;
    guPerspective(&currentTask->projection, &perspNorm, FOV, ASPECT, NEAR_PLANE, FAR_PLANE, 1.0);
    gSPPerspNormalize(displayListPtr++, perspNorm);
    
    gSPMatrix(displayListPtr++, &(currentTask->projection), G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);

    guLookAt(&currentTask->modeview, 
        VEC_GET(current_stage->camera->pos),
        VEC_GET(current_stage->camera->lookAt),
        VEC_GET(current_stage->camera->up)
    );

    gSPMatrix(displayListPtr++, &(currentTask->modeview), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    
    current_stage->stage_frame();

    actors_render();

    debug_printf("Finished Stage-Rendering\n");
}