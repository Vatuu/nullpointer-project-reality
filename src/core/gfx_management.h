#ifndef _GFX_H_
#define _GFX_H_

#define SCREEN_HEIGHT 240
#define SCREEN_WIDTH 320

#define MAX_OBJECTS 64
#define MAX_DL_COMMANDS 2048
#define MAX_GFX_TASKS 5

#define FOV 45
#define ASPECT (f32)SCREEN_WIDTH / (f32)SCREEN_HEIGHT
#define NEAR_PLANE 10
#define FAR_PLANE 1000

typedef struct GfxTask {
    Mtx projection, modeview, objTransform[MAX_OBJECTS];
    Gfx displayList[MAX_DL_COMMANDS];

} GfxTask;

extern Gfx* displayListPtr;
extern GfxTask* currentTask;

extern void gfxSwitchTask();
extern void gfxInitRCP();
extern void gfxClearBuffers();

extern void gfx_function(int pendingGfx);

#endif