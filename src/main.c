#include <nusys.h>
#include "stage00_n64.h"

void mainproc() {
    nuGfxInit();
    nuContInit();
    init();
    nuGfxFuncSet((NUGfxFunc)frame);
    nuGfxDisplayOn();

    while(1);
}