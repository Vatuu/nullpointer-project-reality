#include <nusys.h>
#include <debug.h>
#include "stages.h"

void mainproc() {
    nuGfxInit();
    debug_initialize();
    nuContInit();

    nuGfxFuncSet((NUGfxFunc)gfx_function);

    nuGfxDisplayOn();

    while(1);
}