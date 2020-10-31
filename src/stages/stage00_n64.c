#include <nusys.h>

#include "types.h"
#include "stages.h"
#include "textures.h"

camera cam = {
    {-200.0f, -200.0f, -200.0f},
    { 0.0f, 0.0f, 0.0f },
    { 1.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f}
};

void init() { }

void update() { }

void frame() {
    debug_printf("  Rendering %s\n", "n64");
    //draw_textrect("n64/logo", 5, 5, displayListPtr); 
}

struct stage stage00_n64 = {
    "n64",
    &cam,
    init,
    update,
    frame
};