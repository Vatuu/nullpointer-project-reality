#ifndef _CC_TEXTURES_
#define _CC_TEXTURES_

#include <types.h>
#include <nusys.h>

#define HEADER_SIZE 8

typedef union tex_info {
    u16 width;
    u16 height;
    u8 format;
    u8 size;
    u16 padding;
    char header[8];
} tex_info;

extern u8 _texturesSegmentRomStart[];
extern u8 _texturesSegmentRomEnd[];

extern const size_t TEXTURE_COUNT;
extern data_info TEXTURES[];

extern const tex_info load_texture_dram(const char* id, u32** address);
extern const tex_info load_texture_tmem(const char* id);
extern const void draw_textrect(const char* id, u32 x, u32 y);

#endif