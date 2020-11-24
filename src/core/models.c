#include <string.h>

#include "models.h"

#include "nusys.h"
#include "debug.h"
#include "malloc.h"

void verify_magic(const char* magic, const char* value, const char* error, const char* id) {
    if(strcmp(value, magic) != 0) {
        debug_printf("%s Magic Check for %sfailed! [%s]\n", error, id, value);
        char* str[128];
        sprintf(str, "Magic %s | %s | %s", error, value, id);
        go_commit_die(str);
    }
}

data_info* get_model_info(const char* id) {
    for(size_t i = 0; i < MODEL_COUNT; i++)
        if(strcmp(MODELS[i].id, id) == 0)
            return &MODELS[i];

    return &MODELS[0];
}

Gfx* get_model_dl(const char* id) {
    data_info* info = get_model_info(id);

    if(info->cached_address != NULL)
        return (Gfx*)info->cached_address;

    u32 size = info->end - info->start;
    char* model_data = memalign(64, size * sizeof(char));
    nuPiReadRom((u32)_modelsSegmentRomStart + info->start + 120, model_data, size);

    model_info data;
    memcpy(data.header, model_data, sizeof(model_info));

    verify_magic(HEADER_MAGIC_MODEL, data.magic, "header", id);

    char* vertices_off = (char*)(model_data + data.offset_vertices);
    verify_magic(HEADER_MAGIC_VERTS, vertices_off, "verts", id);
    char* faces_off = (char*)(model_data + data.offset_faces);
    verify_magic(HEADER_MAGIC_FACES, faces_off, "faces", id);
    char* mats_off = (char*)(model_data + data.offset_materials);
    verify_magic(HEADER_MAGIC_MATERIALS, mats_off, "mats", id);
    char* paths_off = (char*)(model_data + data.offset_strings);
    verify_magic(HEADER_MAGIC_STRINGS, paths_off, "paths", id);

    u16 vert_count; 
    vertices_off = vertices_off + 4;
    memcpy(&vert_count, vertices_off, sizeof(u16));
    vertex* vertices = malloc(vert_count * sizeof(vertex));
    for(u16 i = 0; i < vert_count; i++) {
        vertex vert;
        memcpy(&vert, vertices_off + (i * sizeof(vertex)), sizeof(vertex));
        vertices[i] = vert;
    }

    u16 face_count; 
    faces_off = faces_off + 4;
    memcpy(&face_count, faces_off, sizeof(u16));
    face* faces = malloc(face_count * sizeof(face));
    for(u16 i = 0; i < face_count; i++) {
        face fac;
        memcpy(&fac, faces_off + (i * sizeof(face)), sizeof(face));
        faces[i] = fac;
    }

    u16 mat_count; 
    mats_off = mats_off + 4;
    memcpy(&mat_count, mats_off, sizeof(u16));
    material* materials = malloc(mat_count * sizeof(material));
    for(u16 i = 0; i < mat_count; i++) {
        material mat;
        memcpy(&mat, mats_off + (i * sizeof(material)), sizeof(material));
        materials[i] = mat;
    }

    u16 path_count;
    paths_off = paths_off + 4;
    memcpy(&path_count, paths_off, sizeof(u16));
    char* paths[] = malloc(path_count * 64);
    u32 offset = 0;
    for(u16 i = 0; i < path_count; i++) {
        char* str = (paths_off + offset);
        offset += strlen(str);
        paths[i] = str;
    }

    free(model_data);

    //TODO Generate DL

    free(vertices);
    feee(faces);
    free(materials);
    free(paths);

    //TODO Cache DL

    //TODO Return DL
}