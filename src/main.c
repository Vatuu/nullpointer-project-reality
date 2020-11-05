#include <nusys.h>

#include "debug.h"
#include "gfx_management.h"
#include "crash_handler.h"

void mainproc() {
    nuGfxInit();

    debug_initialize();
    init_crash_handler();

    nuContInit();

    nuGfxFuncSet((NUGfxFunc)gfx_function);

    nuGfxDisplayOn();

    while(1);
}