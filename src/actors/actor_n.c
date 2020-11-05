#include "actor.h"
#include "the_n.h"

struct actor_data data = { { 0.0F, 0.0F, 0.0F }, { 0.0F, 0.0F, 0.0F}, 1.0F };

void actor_init() { };

void actor_update() {
    data.rot.yaw = data.rot.yaw + 1 % 360;
};

void actor_frame(f32 tickdelta) {
    gDPSetCycleType(displayListPtr++, G_CYC_1CYCLE);
    gDPSetRenderMode(displayListPtr++, G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2);
    gSPClearGeometryMode(displayListPtr++, 0xFFFFFFFF);
    gSPSetGeometryMode(displayListPtr++, G_SHADE | G_SHADING_SMOOTH | G_ZBUFFER);

    gSPDisplayList(displayListPtr++, N64Yellow_PolyList);
    gSPDisplayList(displayListPtr++, N64Red_PolyList);
    gSPDisplayList(displayListPtr++, N64Blue_PolyList);
    gSPDisplayList(displayListPtr++, N64Green_PolyList);
}

struct actor actor_the_n = {
    "the_n",
    &data,
    actor_init,
    actor_update,
    actor_frame
};