#ifndef _CC_STAGE_
#define _CC_STAGE_

struct StageObject {
    const char* stage_id;
    void (*stage_init)(void);
    void (*stage_update)(void);
    void (*stage_frame)(void);
};

extern void update_stage(struct StageObject newStage);
extern void gfx_function(int pendingGfx);

extern struct StageObject stage00_n64;

#endif