#include <sys/types.h>
#include "CppUTestExt/MockSupport.h"
#include <vector>
#include "write.hpp"

ssize_t wwrite(chenor *self, int fd, const void *buf, size_t count)
{
    const auto cbuf = static_cast<const char*>(buf);
    auto v = new std::vector<char>(cbuf, cbuf + count);
    mock().setDataObject("write_buf", "", v);

    return mock().actualCall("write")
        .withParameter("fd", fd)
        .withParameter("buf", static_cast<const void*>(nullptr))
        .withParameter("count", count)
        .returnIntValue();
}

ssize_t(*write_fp)(chenor *self, int fd, const void *buf, size_t count) = &wwrite;
