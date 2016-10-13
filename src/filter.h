#ifndef _H_FILTER_
#define _H_FILTER_


struct path_filter {
    int len;
    char **patterns;
};

struct path_filter *compile_filter_from_s(const char *pattern);
int match_filter_chunk(struct path_filter *pf, const char *ch, int depth);
#endif
