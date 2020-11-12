#include <textures.h>
#include <string.h>
#include <malloc.h>

#include "gfx_management.h"
#include "debug.h"

static Gfx g_buffer_C_dummy_aligner1[] = { gsSPEndDisplayList() };
char g_buffer[4096] __attribute__((aligned(64)));

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

    char header[6];
    for(size_t i = 0; i < (info->datastart - info->start); i++)
        header[i] = texture_data[i];

    u16 width = *(u16*)&header[0];
    u16 height = *(u16*)&header[2];

    const struct tex_info tex = { width, height, header[4], header[5] };

    *address = (u32*)((char*)info->cached_address + sizeof(header));

    debug_printf("%d, %d, %d, %d\n", tex.width, tex.height, tex.format, tex.size);

    memcpy(g_buffer, *address, 2048);

    debug_printf("Texture Data: %p\n", texture_data);
    debug_printf("Address: %p\n", address);

    return tex;
}

const tex_info load_texture_tmem(const char* id) {
    u32* address = 0;
    tex_info info = load_texture_dram(id, &address);

    debug_printf("G Buffer: %p\n", g_buffer);

    debug_printf("%d, %d, %d, %d\n", info.width, info.height, info.format, info.size);

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

    debug_printf("%d, %d, %d, %d\n", info.width, info.height, info.format, info.size);

    gSPTextureRectangle(displayListPtr++,
        x << 2, (y + 1) << 2,
        (info.width + x - 1) << 2, (info.height + y) << 2,
        0,
        0, 0,
        4 << 10, 1 << 10
    );

    gDPPipeSync(displayListPtr++);
}