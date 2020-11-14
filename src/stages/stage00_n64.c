#include <nusys.h>

#include "stages.h"
#include "textures.h"
#include "debug_font.h"

camera cam = {
    {-200.0f, -200.0f, -200.0f},
    { 0.0f, 0.0f, 0.0f },
    { 1.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f}
};

void init() {
    vec_3d pos = {0.0F, 0.0F, 0.0F};
    rotation_3d rot = {0.0F, 0.0F, 0.0F};
    vec_3d pos1 = {0.0F, 30.0F, 0.0F};
    rotation_3d rot1 = {0.0F, 0.0F, 90.0F};
    vec_3d pos2 = {0.0F, 0.0F, 40.0F};
    rotation_3d rot2 = {0.0F, 0.0F, 0.0F};
    actor_create(ACTORS, &actor_the_n, pos, rot, 1.0F, false);
    actor_create(ACTORS, &actor_the_n, pos1, rot1, 1.0F, false);
    actor_create(ACTORS, &actor_the_n, pos2, rot2, 1.0F, false);   
}

void update() { }

void frame() {
    debug_printf("  Rendering %s\n", "n64");
    draw_textrect("n64/mask", 48, 48); 
}

struct stage stage00_n64 = {
    "n64",
    &cam,
    init,
    update,
    frame
};