#include "stages.h"
#include "debug.h"

struct stage* current_stage = &stage00_n64;
struct actor* current_actor = &actor_the_n;

void change_stage(struct stage newStage) {
    current_stage = &newStage;
    current_stage->stage_init();
}

void actors_update() {
    current_actor->actor_update();
}

void actors_render() {
    debug_printf("  Starting Actor-Rendering\n");

    for(size_t i = 0; i < 1; i++) {
        debug_printf("    Rendering %s\n", current_actor->actor_id);

        guPosition(
            &currentTask->objTransform[i],
            current_actor->data->rot.roll, current_actor->data->rot.pitch, current_actor->data->rot.yaw, current_actor->data->scale,
            VEC_GET(current_actor->data->position)
        );

        gSPMatrix(displayListPtr++, &(currentTask->objTransform[i]), G_MTX_MODELVIEW | G_MTX_PUSH | G_MTX_MUL);

        current_actor->actor_frame(1.0F);

        gDPPipeSync(displayListPtr++);

        gSPPopMatrix(displayListPtr++, G_MTX_MODELVIEW);
    }

    debug_printf("  Finished Actor-Rendering\n");
}

void stage_update() {
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