#include <nusys.h>

#include "stages.h"
#include "textures.h"
#include "debug_font.h"

camera cam = {
    {-200.0f, -200.0f, -200.0f},
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, -1.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f}
};

void init() {
    vec_3d pos = {0.0F, 0.0F, 0.0F};
    rotation_3d rot = {0.0F, 0.0F, 0.0F};
    actor_physical_create(ACTORS, &actor_the_n, true, pos, rot, .01F, true);
    //actor_create(ACTORS, &actor_the_n, pos, rot1, .01F, false);
    //actor_create(ACTORS, &actor_the_n, pos, rot2, .01F, false);   
}

void update() { }

void frame() {
    //draw_textrect("n64/mask", 48, 48); 
}

struct stage stage00_n64 = {
    "n64",
    &cam,
    init,
    update,
    frame
};