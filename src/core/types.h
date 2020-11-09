#ifndef _CCTYPES_
#define _CCTYPES_

#include <PR/ultratypes.h>

#define VEC_GET(v) v.x, v.x, v.z

#define FB_ADDRESS 0xA038F800

typedef struct vec_3d { float x, y, z; } vec_3d;
typedef struct rotation { float roll, yaw, pitch; } rotation;

typedef struct camera {
    vec_3d pos, lookAt, up;
    rotation rot;
} camera;

typedef struct data_info
{
    const char* id;
    const u32 start, datastart, end;
    u32 cached_address;
} data_info;

#endif