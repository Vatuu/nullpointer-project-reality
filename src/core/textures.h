#ifndef _CC_TEXTURES_
#define _CC_TEXTURES_

#include <types.h>

typedef struct tex_info {
    data_info data;
    u16 width, height;
    u8 format, size;
} tex_info;

extern char _texturesSegmentRomStart[];
extern char _texturesSegmentRomEnd[];

extern const size_t TEXTURE_COUNT;
extern const tex_info TEXTURES[];

extern const tex_info* get_texture_info(const char* id);
extern const u8* get_texture(const char* id, size_t* size);

#endif