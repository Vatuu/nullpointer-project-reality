#include <nusys.h>
#include <assert.h>

#include "gfx_management.h"
#include "stages.h"

GfxTask graphicsTasks[MAX_GFX_TASKS];

Gfx* displayListPtr;
GfxTask* currentTask;

int gfxTaskNum = 0;

static Vp vp = {
    SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2, G_MAXZ / 2, 0, // Scalefactor
    SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2, G_MAXZ / 2, 0, // Move
};

Gfx setup_rdpstate[] = {
    gsDPSetRenderMode(G_RM_OPA_SURF, G_RM_OPA_SURF2),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPSetScissor(G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT),
    gsDPSetColorDither(G_CD_BAYER),
    gsSPEndDisplayList()
};

Gfx setup_rspstate[] = {
    gsSPViewport(&vp),
    gsSPClearGeometryMode(0xFFFFFFFF),
    gsSPSetGeometryMode(G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH | G_CULL_BACK),
    gsSPTexture(0, 0, 0, 0, G_OFF),
    gsSPEndDisplayList()
};

void gfxSwitchTask() {
    gfxTaskNum = (gfxTaskNum + 1) % MAX_GFX_TASKS;
    currentTask = &graphicsTasks[gfxTaskNum];
    displayListPtr = &currentTask->displayList[0];
}

void gfxInitRCP() {
    gSPSegment(displayListPtr++, 0, 0x0);
    gSPDisplayList(displayListPtr++, OS_K0_TO_PHYSICAL(setup_rdpstate));
    gSPDisplayList(displayListPtr++, OS_K0_TO_PHYSICAL(setup_rspstate));
}

void gfxClearBuffers() {
    // Clear the Z-Buffer
    gDPSetDepthImage(displayListPtr++, OS_K0_TO_PHYSICAL(nuGfxZBuffer));
    gDPSetCycleType(displayListPtr++, G_CYC_FILL);
    gDPSetColorImage(displayListPtr++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WIDTH, OS_K0_TO_PHYSICAL(nuGfxZBuffer));
    gDPSetFillColor(displayListPtr++, (GPACK_ZDZ(G_MAXFBZ, 0) << 16 | GPACK_ZDZ(G_MAXFBZ, 0)));
    gDPFillRectangle(displayListPtr++, 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
    gDPPipeSync(displayListPtr++);
    // Clear the Framebuffer
    gDPSetColorImage(displayListPtr++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WIDTH, osVirtualToPhysical(nuGfxCfb_ptr));
    gDPSetFillColor(displayListPtr++, (GPACK_RGBA5551(0, 0, 0, 1) << 16 | GPACK_RGBA5551(0, 0, 0, 1)));
    gDPFillRectangle(displayListPtr++, 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
    gDPPipeSync(displayListPtr++);
}

int yaw = 0;

void gfx_function(int pendingGfx) {
    stage_update();

    if(pendingGfx >= 1) {
        return;
    }

    gfxSwitchTask();

    gfxInitRCP();
    gfxClearBuffers();

    stage_render();

    gDPFullSync(displayListPtr++);
    gSPEndDisplayList(displayListPtr++);

    assert(displayListPtr - currentTask->displayList < MAX_DL_COMMANDS);

    nuGfxTaskStart(
        currentTask->displayList,
        (int)(displayListPtr - currentTask->displayList) * sizeof(Gfx),
        NU_GFX_UCODE_F3DEX2,
        NU_SC_SWAPBUFFER
    );
}
