#pragma once

#include <sys/types.h>
class chenor;
extern ssize_t(*write_fp)(chenor *self, int fd, const void *buf, size_t count);
