#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

char* readFile(const char* filename, size_t* size);
uint32_t countLines(char* text, size_t file_size);

#endif
