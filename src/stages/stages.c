#include "stages.h"

struct StageObject* current_stage;

void update_stage() {

}

void gfx_function(int gfxPending) {
    current_stage->stage_update();
    if(gfxPending < 1)
        current_stage->stage_frame();
}