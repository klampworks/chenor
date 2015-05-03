#include <unistd.h>
ssize_t(*write_fp)(int fd, const void *buf, size_t count) = &write;
