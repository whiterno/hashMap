#include <assert.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "utils.h"

char* readFile(const char* filename, size_t* size){
    assert(filename);

    struct stat file_stat = {};
    if ((stat(filename, &file_stat)) == -1) return NULL;

    size_t file_size = file_stat.st_size;
    *size = file_size;

    int file_descr = open(filename, O_RDONLY);
    if (file_descr == -1) return NULL;

    char* text = (char*)calloc(file_size, sizeof(char));
    if (!text) return NULL;

    read(file_descr, text, file_size);

    close(file_descr);

    return text;
}

uint32_t countLines(char* text, size_t file_size){
    assert(text);

    char* new_text = text;
    uint32_t lines = 0;

    while(new_text - text != file_size){
        lines++;
        new_text  = strchr(new_text, '\n') + 1;
    }

    return lines;
}
