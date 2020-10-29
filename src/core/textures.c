#include <textures.h>
#include <string.h>

const data_info* get_texture_info(const char* id) {
    for(size_t i = 0; i < TEXTURE_COUNT; i++)
        if(strcmp(TEXTURES[i].id, id) == 0)
            return &TEXTURES[i];

    return &TEXTURES[0];
}

const tex_info load_texture_dram(const char* id, u32* address) {
    const data_info* info = get_texture_info(id);
    u32 size = info->end - info->start;

    u32 buff_size = size;

    char buffer[buff_size] __attribute__((aligned(8)));
    nuPiReadRom((u32)_texturesSegmentRomStart + info->start, buffer, size);

    *address = (u32)buffer;

    char header[6];
    for(size_t i = 0; i < info->datastart - info->start; i++)
        header[i] = buffer[i];

    short width, height;
    memcpy(&width, &header[0], sizeof(u16));
    memcpy(&height, &header[2], sizeof(u16));
    const struct tex_info tex = { width, height, header[4], header[5] };

    return tex;
}

const tex_info load_texture_tmem(const char* id, Gfx* displayListPtr) {
    u32 address;
    tex_info info = load_texture_dram(id, &address);

    gSPSegment(displayListPtr++, 0, 0);
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

const void draw_textrect(const char* id, u32 x, u32 y, Gfx* dlistptr) {
    gDPPipeSync(dlistptr++);
    gDPSetTexturePersp(dlistptr++, G_TP_NONE);
    gDPSetCycleType(dlistptr++, G_CYC_COPY);
    gDPSetRenderMode(dlistptr++, G_RM_NOOP, G_RM_NOOP2);

    gSPClearGeometryMode(dlistptr++, G_SHADE | G_SHADING_SMOOTH);
    gSPTexture(dlistptr++, 0x2000, 0x0200, 0, G_TX_RENDERTILE, G_ON);

    gDPSetCombineMode(dlistptr++, G_CC_DECALRGB, G_CC_DECALRGB);
    gDPSetTexturePersp(dlistptr++, G_TP_NONE);
    gDPSetTextureFilter(dlistptr++, G_TF_POINT);

    tex_info info = load_texture_tmem(id, dlistptr);

    gSPTextureRectangle(dlistptr++,
        x, y,
        info.width + x, info.height + y,
        0,
        0, 0,
        4 << 10, 1 << 10
    );

    gDPPipeSync(dlistptr++);
}