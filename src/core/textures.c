#include <textures.h>

int strcmp(const char *s1, const char *s2) {
    int c1, c2;
    do {
        c1 = (unsigned char)*s1++;
        c2 = (unsigned char)*s2++;
    } while (c1 != '\0' && c1 == c2);
    return c1 - c2;
}

const tex_info* get_texture_info(const char* id) {
    for(size_t i = 0; i < TEXTURE_COUNT; i++)
        if(strcmp(TEXTURES[i].data.id, id) == 0)
            return &TEXTURES[i];

    return &TEXTURES[0];
}

const u8* get_texture_address(const char* id, size_t* size) {
    //tex_info* info = get_texture_info(id);
    //if(size != NULL)
        //size = info->end - *info->start;
    return 0;//_texturesSegmentRomStart + info->start;
}