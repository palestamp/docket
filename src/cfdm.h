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
    off_t size;
    char *map;
};


int map_file(const char *filename, struct cfdmap* cfdmap);

#endif
