#ifndef _CC_STAGE_
#define _CC_STAGE_

#include "actor.h"
#include "gfx_management.h"

struct stage {
    const char* stage_id;
    camera* camera;
    void (*stage_init)(void);
    void (*stage_update)(void);
    void (*stage_frame)(void);
};

extern void change_stage(struct stage newStage);

extern void actor_update();
extern void actor_render(GfxTask* task);
extern void stage_update();
extern void stage_render(GfxTask* task);

extern struct stage stage00_n64;

#endif