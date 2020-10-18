#include "stages.h"

struct StageObject* current_stage = &stage00_n64;

void update_stage(struct StageObject newStage) {
    current_stage = &newStage;
    current_stage->stage_init();
}

void gfx_function(int gfxPending) {
    current_stage->stage_update();
    if(gfxPending < 1)
        current_stage->stage_frame();
}