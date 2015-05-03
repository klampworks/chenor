#pragma once

#include <sys/types.h>
extern ssize_t(*write_fp)(int fd, const void *buf, size_t count);
