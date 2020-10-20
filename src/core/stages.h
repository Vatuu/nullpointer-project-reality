#ifndef _CC_STAGE_
#define _CC_STAGE_

struct stage {
    const char* stage_id;
    void (*stage_init)(void);
    void (*stage_update)(void);
    void (*stage_frame)(void);
};

extern void update_stage(struct stage newStage);
extern void gfx_function(int pendingGfx);

extern struct stage stage00_n64;

#endif