#ifndef _CC_MODELS_
#define _CC_MODELS_

#include "types.h"

#define HEADER_SIZE_MODEL 20
#define HEADER_MAGIC_MODEL "RBM\0"
#define HEADER_MAGIC_VERTS "VTX\0"
#define HEADER_MAGIC_FACES "FAC\0"
#define HEADER_MAGIC_MATERIALS "MAT\0"
#define HEADER_MAGIC_STRINGS "STR\0"

typedef union vertex_value {
    vec_3s vector_normal;
    u16 vector_rgb[3];
    char value[6];
} vertex_value;

typedef struct vertex {
    vec_3s position;
    vertex_value vertex_data;
    u16 u, v;
} vertex;

typedef struct material {
    char* texture_id;
    char geometry_mode;
} material;

typedef struct face {
    material mat;
    vertex vertices[3];
    char type, alpha;
} face;

typedef union model_info {
    char magic[4];
    u32 offset_vertices;
    u32 offset_faces;
    u32 offset_materials;
    u32 offset_strings;
    char header[20];
} model_info;

extern u8 _modelsSegmentRomStart[];
extern u8 _modelsSegmentRomEnd[];

extern const size_t MODEL_COUNT;
extern data_info MODELS[];

#endif