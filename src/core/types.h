#ifndef _CCTYPES_
#define _CCTYPES_

#include <PR/ultratypes.h>

#define VEC_GET(v) v.x, v.x, v.z
#define ROT_GET(r) r.roll, r.yaw, r.pitch

#define true 1
#define false 0

typedef u8 bool; 

typedef struct vec_3d { f32 x, y, z; } vec_3d;
typedef struct rotation_3d { f32 roll, yaw, pitch; } rotation_3d;

typedef struct camera {
    vec_3d pos, lookAt, up;
    rotation_3d rot;
} camera;

typedef struct data_info
{
    const char* id;
    const u32 start, datastart, end;
    u32 cached_address;
} data_info;

#endif