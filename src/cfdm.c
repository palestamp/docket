#include <stdlib.h>
#include <stdio.h>

#include "cfdm.h"

char *
map_file(const char *filename, size_t *len) {
    int fd;
    struct stat sb;

    if((fd = open(filename, O_RDONLY)) < 0) {
        perror("open");
        return 0;
    }

    if(stat(filename, &sb) != 0) {
        perror("fstat");
        return 0;
    }
    if(!S_ISREG(sb.st_mode)) {
        fprintf(stderr, "%s is not a file\n", filename);
        return 0;
    }
    char *map = mmap(0, sb.st_size, PROT_READ, MAP_PRIVATE , fd, 0);
    if(close(fd) == -1) {
        perror("close");
        return 0;
    }
    if (map == MAP_FAILED) {
        perror("mmap");
        return 0;
    }

    *len = sb.st_size;
    return map;
}

void
map(const char *filename, struct cfdmap *c) {
    size_t len = 0;
    c->map = map_file(filename, &len);
    c->size = len;
}


