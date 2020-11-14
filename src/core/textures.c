#include <textures.h>
#include <string.h>
#include <malloc.h>

#include "gfx_management.h"
#include "debug.h"

data_info* get_texture_info(const char* id) {
    for(size_t i = 0; i < TEXTURE_COUNT; i++)
        if(strcmp(TEXTURES[i].id, id) == 0)
            return &TEXTURES[i];

    return &TEXTURES[0];
}

const tex_info load_texture_dram(const char* id, u32** address) {
    data_info* info = get_texture_info(id);

    char* texture_data;

    if(info->cached_address == NULL) {
        u32 size = info->end - info->start;

        texture_data = memalign(64, size * sizeof(char));
        nuPiReadRom((u32)_texturesSegmentRomStart + info->start + 120, texture_data, size);

        info->cached_address = (u32)texture_data;

    } else {
        texture_data = (char*)info->cached_address;
    }

    tex_info data;
    memcpy(data.header, texture_data, HEADER_SIZE);

    *address = (u32*)((char*)info->cached_address + HEADER_SIZE);

    return data;
}

const tex_info load_texture_tmem(const char* id) {
    u32* address = 0;
    tex_info info = load_texture_dram(id, &address);

    gDPLoadTextureBlock(displayListPtr++,
        address,
        G_IM_FMT_RGBA, G_IM_SIZ_16b,
        info.width, info.height,
        0,
        G_TX_NOMIRROR, G_TX_NOMIRROR,
        G_TX_NOMASK, G_TX_NOMASK,
        G_TX_NOLOD, G_TX_NOLOD
    );

    return info;
}



const void draw_textrect(const char* id, u32 x, u32 y) {
    gDPPipeSync(displayListPtr++);
    gDPSetTexturePersp(displayListPtr++, G_TP_NONE);
    gDPSetCycleType(displayListPtr++, G_CYC_COPY);
    gDPSetRenderMode(displayListPtr++, G_RM_NOOP, G_RM_NOOP2);

    gSPClearGeometryMode(displayListPtr++, G_SHADE | G_SHADING_SMOOTH);
    gSPTexture(displayListPtr++, 0x2000, 0x0200, 0, G_TX_RENDERTILE, G_ON);

    gDPSetCombineMode(displayListPtr++, G_CC_DECALRGB, G_CC_DECALRGB);
    gDPSetTexturePersp(displayListPtr++, G_TP_NONE);
    gDPSetTextureFilter(displayListPtr++, G_TF_POINT);

    tex_info info = load_texture_tmem(id);

    gSPTextureRectangle(displayListPtr++,
        x << 2, (y + 1) << 2,
        (info.width + x - 1) << 2, (info.height + y) << 2,
        0,
        0, 0,
        4 << 10, 1 << 10
    );

    gDPPipeSync(displayListPtr++);
}