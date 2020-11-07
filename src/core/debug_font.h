#ifndef _CC_DEBUGFONT_
#define _CC_DEBUGFONT_

#include "types.h"

void debug_draw_char(u16* buffer, u32 x, u32 y, char c);
void debug_draw_string(u16* buffer, u32 x, u32 y, char* string);

#endif