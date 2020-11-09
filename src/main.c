#include <nusys.h>
#include <malloc.h>

#include "debug.h"
#include "gfx_management.h"
#include "crash_handler.h"

extern u8 _codeSegmentEnd[];

void mainproc() {
    nuGfxInit();
    InitHeap((void *)_codeSegmentEnd, 0x8033F800 - (u32)_codeSegmentEnd);

    debug_initialize();
    init_crash_handler();

    nuContInit();

    nuGfxFuncSet((NUGfxFunc)gfx_function);

    nuGfxDisplayOn();

    while(1);
}