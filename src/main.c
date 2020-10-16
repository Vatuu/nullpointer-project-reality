#include <nusys.h>
#include "stages.h"



void mainproc() {
    nuGfxInit();
    nuContInit();

    nuGfxFuncSet((NUGfxFunc)gfx_function);

    nuGfxDisplayOn();

    while(1);
}