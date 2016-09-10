#ifndef H_MAPPER
#define H_MAPPER

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>


/*
 * Storage for memmory mapped file
 */

struct cfdmap {
    unsigned char *map;
    size_t size;
};


unsigned char* map_file(const char *filename, size_t *len);
void map(const char *filename, struct cfdmap *c);

#endif
