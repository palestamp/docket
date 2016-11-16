#ifndef DOCKET_LOG_H
#define DOCKET_LOG_H

void die_error(const char *err, ...) __attribute__((format (printf, 1, 2)));
void die_fatal(const char *err, ...) __attribute__((format (printf, 1, 2)));
void usage_and_die(const char *err, ...) __attribute__((format (printf, 1, 2)));

#endif
