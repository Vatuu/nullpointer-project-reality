#include <assert.h>
#include <nusys.h>

#include "gfx_management.h"
#include "types.h"
#include "the_n.h"

Vec3d cameraPos = {-200.0f, -200.0f, -200.0f};
Vec3d cameraTarget = { 0.0f, 0.0f, 0.0f };
Vec3d cameraUp = { 0.0f, 1.0f, 0.0f };

int yaw;


void init() {
    yaw = 0;
}

void update() {
    yaw = yaw + 1 % 360;
}

void drawScreen() {
    unsigned short perspNorm;
    GfxTask *task = gfxSwitchTask();

    gfxInitRCP();
    gfxClearBuffers();

    guPerspective(&task->projection, &perspNorm, FOV, ASPECT, NEAR_PLANE, FAR_PLANE, 1.0);
    gSPPerspNormalize(displayListPtr++, perspNorm);

    guLookAt(&task->modeview, 
        cameraPos.x, cameraPos.y, cameraPos.z,
        cameraTarget.x, cameraTarget.y, cameraTarget.z,
        cameraUp.x, cameraUp.y, cameraUp.z
    );

    gSPMatrix(displayListPtr++, OS_K0_TO_PHYSICAL(&(task->projection)), G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPMatrix(displayListPtr++, OS_K0_TO_PHYSICAL(&(task->modeview)), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);

    guPosition(
        &task->objTransform[0],
        0.0f, 0.0f, yaw, 1.0f,
        0.0f, 0.0f, 0.0f 
    );

    gSPMatrix(displayListPtr++, OS_K0_TO_PHYSICAL(&(task->objTransform[0])), G_MTX_MODELVIEW | G_MTX_PUSH | G_MTX_MUL);
    
    gDPSetCycleType(displayListPtr++, G_CYC_1CYCLE);
    gDPSetRenderMode(displayListPtr++, G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2);
    gSPClearGeometryMode(displayListPtr++, 0xFFFFFFFF);
    gSPSetGeometryMode(displayListPtr++, G_SHADE | G_SHADING_SMOOTH | G_ZBUFFER);

    gSPDisplayList(displayListPtr++, N64Yellow_VertList);
    gSPDisplayList(displayListPtr++, N64Red_VertList);
    gSPDisplayList(displayListPtr++, N64Blue_VertList);
    gSPDisplayList(displayListPtr++, N64Green_VertList);

    gDPPipeSync(displayListPtr++);

    gSPPopMatrix(displayListPtr++, G_MTX_MODELVIEW);

    gDPFullSync(displayListPtr++);
    gSPEndDisplayList(displayListPtr++);

    assert(displayListPtr - task->displayList < MAX_DL_COMMANDS);

    nuGfxTaskStart(
        task->displayList,
        (int)(displayListPtr - task->displayList) * sizeof(Gfx),
        NU_GFX_UCODE_F3DEX2,
        NU_SC_SWAPBUFFER
    );
}

void frame(int pendingGfx) {
    if(pendingGfx < 1)
        drawScreen();
    update();    
}