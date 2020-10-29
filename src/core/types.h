#ifndef _CCTYPES_
#define _CCTYPES_

#include <PR/ultratypes.h>

typedef struct vec_3d { float x, y, z; } vec_3d;

typedef struct data_info
{
    const char* id;
    const u32 start, datastart, end;
} data_info;

#endif