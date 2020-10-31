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

void actors_render(GfxTask* task) {
    debug_printf("  Starting Actor-Rendering\n");

    for(size_t i = 0; i < 1; i++) {
        debug_printf("    Rendering %s\n", current_actor->actor_id);
        gSPMatrix(displayListPtr++, OS_K0_TO_PHYSICAL(&(task->objTransform[i])), G_MTX_MODELVIEW | G_MTX_PUSH | G_MTX_MUL);

        guPosition(
            &task->objTransform[i],
            current_actor->data->rot.roll, current_actor->data->rot.pitch, current_actor->data->rot.yaw, current_actor->data->scale,
            current_actor->data->position.x, current_actor->data->position.y, current_actor->data->position.z
        );

        current_actor->actor_frame(&task->objTransform[i], displayListPtr, 1.0F);

        gDPPipeSync(displayListPtr++);

        gSPPopMatrix(displayListPtr++, G_MTX_MODELVIEW);
    }
    debug_printf("  Finished Actor-Rendering\n");
}

void stage_update() {
    current_stage->stage_update();
    actors_update();
}

void stage_render(GfxTask* task) {
    debug_printf("Starting Stage-Rendering\n");
    
    guLookAt(&task->modeview, 
        current_stage->camera->pos.x, current_stage->camera->pos.y, current_stage->camera->pos.z,
        current_stage->camera->lookAt.x, current_stage->camera->lookAt.y, current_stage->camera->lookAt.z,
        current_stage->camera->up.x, current_stage->camera->up.y, current_stage->camera->up.z
    );

    current_stage->stage_frame();
    actors_render(task);
    debug_printf("Finished Stage-Rendering\n");
}