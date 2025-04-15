#include <assert.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "string_t.h"
#include "crc32_hash.h"

char* readFile(const char* filename){
    assert(filename);

    struct stat file_stat = {};
    if ((stat(filename, &file_stat)) == -1) return NULL;

    size_t file_size = file_stat.st_size;

    int file_descr = open(filename, O_RDONLY);
    if (file_descr == -1) return NULL;

    char* text = (char*)calloc(file_size, sizeof(char));
    if (!text) return NULL;

    read(file_descr, text, file_size);

    close(file_descr);

    return text;
}

String_t* buildStringArray(char* text){
    assert(text);
}
