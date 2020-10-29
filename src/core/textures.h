#ifndef _CC_TEXTURES_
#define _CC_TEXTURES_

#include <types.h>
#include <nusys.h>

typedef struct tex_info {
    u16 width, height;
    u8 format, size;
} tex_info;

extern u8 _texturesSegmentRomStart[];
extern u8 _texturesSegmentRomEnd[];

extern const size_t TEXTURE_COUNT;
extern const data_info TEXTURES[];

extern const data_info* get_data_info(const char* id);
extern const tex_info load_texture_dram(const char* id, u32* address);
extern const tex_info load_texture_tmem(const char* id, Gfx* displayListPtr);
extern const void draw_textrect(const char* id, u32 x, u32 y, Gfx* dlistptr);

#endif