#include <unistd.h>
#error
ssize_t(*write_fp)(int fd, const void *buf, size_t count) = &write;
