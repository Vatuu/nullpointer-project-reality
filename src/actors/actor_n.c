#include "actor.h"
#include "the_n.h"

void actor_init(actor_data* data) {
    physical(data)->rotation.yaw = 0;
};

void actor_update(actor_data* data) {
    physical(data)->rotation.yaw = physical(data)->rotation.yaw + 1 % 360;
};

void actor_frame(actor_data* data, f32 tickdelta) {
    gDPSetCycleType(displayListPtr++, G_CYC_1CYCLE);
    gDPSetRenderMode(displayListPtr++, G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2);
    gDPSetCombineMode(displayListPtr++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gSPClearGeometryMode(displayListPtr++, 0xFFFFFFFF);
    gSPTexture(displayListPtr++, 0, 0, G_TX_NOLOD, G_TX_RENDERTILE, G_OFF);
    gSPSetGeometryMode(displayListPtr++, G_SHADE | G_SHADING_SMOOTH | G_ZBUFFER);
    
    gSPDisplayList(displayListPtr++, &Wtx_N64Logo);
}

struct physical_actor actor_the_n_physical = {
        actor_frame,
        NULL
};

struct actor actor_the_n = {
    "the_n",
    actor_init,
    actor_update,
    &actor_the_n_physical
};