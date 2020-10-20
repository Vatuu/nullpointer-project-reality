#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fts.h>

#define VERSION "1.0"

#define TYPE_TEXTURE() strcmp(type, "texture") == 0

struct buffer {
    char* data;
    size_t size, alloc;
};

struct data_entry {
    const char* data_name;
    char* start, datastart, end;
};

struct texture_header {
    char* texture_name;
    unsigned short width, height;
    unsigned char format, size;
};

char* template[] = {
    "#include <%s.h>\n\n",
    "const size_t %s = %zu\n\n",
    "const tex_info %s[] = {\n",
    "   %s\n",
    "};"
};

char* input_path, output_path, c_path;
char* type, extension;
char* texmap_path;

struct texture_header* texheaders;

char* get_string_from_file(FILE* file) {
    char* buffer = 0;
    size_t length;
    
    fseek(file, 0, SEEK_END);
    length = ftell(file);
    fseek(file, 0, SEEK_SET);
    buffer = malloc(length);
    fread(buffer, 1, length, file);

    return buffer;
}

size_t create_texture_header_array() {
    FILE* map = fopen(texmap_path, "r");
    size_t entries = 0;
    char c;
    
    if(map == NULL) {
        printf("Unable to open Texture Metadata Map at \"%s\"!", texmap_path);
        exit(1);
    }

    for(c = getc(map); c != EOF; c = getc(map))
        if(c == '\n')
            entries++;
    
    struct texture_header* headers[entries];
    char* mapdata = get_string_from_file(map);
    fclose(map);

    char* line = strok(mapdata, '\n');
    for(size_t i = 0; line != NULL; i++) {
        char* metadata[3];
        struct texture_header* header;
    
        char* ptr = strtok(line, ' ');
        for(size_t i = 0; ptr != NULL; i++) {
            metadata[i] = ptr;
            ptr = strtok(NULL, ' ');
        }
    
        header->texture_name = metadata[0];
        header->format = strok(metadata[1], '/');
        header->size = strtok(NULL, '/');
        header->width = strtok(metadata[2], 'x');
        header->height = strtok(NULL, 'x');

        headers[i] = header;
        line = strok(NULL, '\n');
    }

    texheaders = headers;
    return entries;
}

struct texture_header* get_texture_header(char* path) {
    for(size_t i = 0; i < sizeof(texheaders); i++)
        if(strcmp(texheaders[i].texture_name, path) == 0)
            return &texheaders[i];
}

void append_data(struct buffer *buf, void *data, size_t size) {
    if (size > buf->alloc - buf->size) {
        size_t new_alloc = buf->alloc;
        if (new_alloc == 0) {
            new_alloc = 64;
        }
        while (size > new_alloc - buf->size) {
            new_alloc *= 2;
        }
        char *new_data = realloc(buf->data, new_alloc);
        if (new_data == NULL) {
            printf("Out of memory!");
            exit(1);
        }
        buf->data = new_data;
        buf->alloc = new_alloc;
    }
    memcpy(buf->data + buf->size, data, size);
    buf->size += size;
}

int endsWith(const char* str, const char* suffix) {
    if(!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix >  lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

void remove_substring(char* string, const char* substring) {

    while(string = strstr(string, substring))
        memmove(string, string + strlen(substring), 1 + strlen(string + strlen(substring)));
}

void print_version() {
    printf("Blobinator %s by Nicolas \"Vatuu\" Adamoglou\n", VERSION);
    printf("  A tool to package data assets and generate a identifier-based\n");
    printf("  C file for accessing data without static definitions.\n\n");
}

void print_usage() {
    print_version();
    printf("Options:\n");
    printf("  -i <Include Directory Path>\n");
    printf("     Directory containing the files to be included.\n");
    printf("  -t [TEXTURE|MODEL|SOUND]\n");
    printf("     Defines the data type to be blobbed.\n     Will be used by the generated C template.\n");
    printf("  -o <Path>\n");
    printf("     Path at which the blobfish will be generated.\n");
    printf("  -e <Extension>\n");
    printf("     Extension of the files to be blobbed.\n");
    printf("  -m\n");
    printf("     Path at which the C file will be generated.\n");
    printf("  -d\n");
    printf("     Path to the texture metadata map (TEXTURES only).\n");
    printf("  -v\n");
    printf("     Prints the version and description of the Blobinator.\n");
    printf("  -h -?\n");
    printf("     Prints this message.\n");
}

int main(int argc, char* argv[]) {
    int option;
    while((option = getopt(argc, argv, "i:t:e:o:v?h")) != -1) {
        switch(option) {
            case 'i':
                input_path = optarg;
                break;
            case 't':
                type = optarg;
                break;
            case 'm':
                c_path = optarg;
                break;
            case 'd':
                texmap_path = optarg;
                break;
            case 'o':
                output_path = optarg;
                break;
            case 'e': 
                extension = optarg;
                break;
            case 'v':
                print_version();
                return 0;
            case '?':
            case 'h':
            default:
                print_usage();
                return 0;
        }
    }



    char* dirs[2] = {input_path, NULL};
    FTS* ftsp = fts_open(dirs, FTS_LOGICAL, NULL);
    if(ftsp == NULL) {
        printf("Unable to open directory \"%s\"!", input_path);
        return 1;
    }

    size_t count = 0;
    struct buffer buf = {};


    if(TYPE_TEXTURE()) 
        count = create_texture_header_array();
    
    char* centries = "";

    FTSENT* ent;
    for(size_t i = 0; (ent = fts_read(ftsp)) != NULL && i < count; i++) {
        char* path = ent->fts_accpath;
        if(endsWith(path, extension)) {

            size_t start = buf.size;

            if(TYPE_TEXTURE()) {
                struct texture_header* header = get_texture_header(path);
                append_data(&buf, header->format, sizeof(unsigned char));
                append_data(&buf, header->size, sizeof(unsigned char));
                append_data(&buf, header->width, sizeof(unsigned short));
                append_data(&buf, header->height, sizeof(unsigned short));
            }

            count++;
            size_t size, headersize;
            FILE *data = fopen(path, "rb");

            fseek(data, 0, SEEK_END);
            size = ftell(data);
            fseek(data, 0, SEEK_SET);

            char* tmpBuf[32 * 1024];
            while (1) {
                size_t r = fread(tmpBuf, 1, sizeof(tmpBuf), data);
                if (r == 0)
                    break;
                append_data(&buf, tmpBuf, r);
            }
            
            if((size & 1)) {
                append_data(&buf, 0x00, 1);
                size++;
            }

            if(TYPE_TEXTURE())
                size += 6;

            size_t datastart = start + (TYPE_TEXTURE() ? 6 : 0);
            size_t dataend = buf.size;

            remove_substring(path, input_path); 
            const char* formatted;
            sprintf(formatted, "{\"%s\", %zu, %zu, %zu},\n", strok(path, '.'), start, datastart, dataend);
            strcat(centries, formatted);

            fclose(data);
        }
    }

    fts_close(ftsp);

    FILE* output_blob = fopen(output_path, "wb");
    if(output_blob == NULL) {
        printf("Unable to create output blob file \"%s\"", output_path);
        exit(1);
    }
    fwrite(buf.data, 1, buf.size, output_blob);
    fclose(output_blob);

    if(TYPE_TEXTURE()) {
        sprintf(template[0], "textures");
        sprintf(template[1], "TEXTURE_COUNT", count);
        sprintf(template[2], "TEXTURES");
        sprintf(template[3], centries);
    }

    FILE* output_c = fopen(c_path, "w+");
        if(output_blob == NULL) {
        printf("Unable to create output c file \"%s\"", output_path);
        exit(1);
    }

    fputs(template[0], output_blob);
    fputs(template[1], output_blob);
    fputs(template[2], output_blob);
    fputs(template[3], output_blob);
    fputs(template[4], output_blob);

    fclose(output_c);

    printf("DONE!");

    return 0;
}
