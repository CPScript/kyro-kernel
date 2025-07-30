#ifndef UNISTD_H
#define UNISTD_H

#include <stdbool.h>

int read(int fd, void *buf, size_t count);
int write(int fd, const void *buf, size_t count);

#endif
