#include <stdlib.h>
#include <stdio.h>

#include "cfdm.h"

int
map_file(const char *filename, struct cfdmap* cfdm) {
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
    cfdm->map = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (cfdm->map == MAP_FAILED) {
        perror("mmap");
        return 0;
    }

    if(close(fd) == -1) {
        perror("close");
        return 0;
    }

    cfdm->size = sb.st_size;
    return 1;
}
