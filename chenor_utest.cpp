#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTestExt/MockSupport.h"
#include "chenor.hpp"
#include <vector>
//#include <unistd.h>

TEST_GROUP(chenor_write)
{
    const std::vector<char> *write_buf;

    void setup()
    {
        write_buf = nullptr;
    }

    void teardown()
    {
        if (!write_buf) {
            write_buf = static_cast<const std::vector<char>*>(
                mock().getData("buf").getObjectPointer());
        }

        delete write_buf;
        mock().clear();
    }
};

ssize_t write(int fd, void *buf, size_t count)
{
    const auto cbuf = static_cast<char*>(buf);
    auto v = new std::vector<char>(cbuf, cbuf + count);
    mock().setDataObject("buf", "", v);

    return mock().actualCall("write")
        .withParameter("fd", fd)
        .withOutputParameter("buf", buf)
        .withParameter("count", count)
        .returnIntValue();
}

TEST(chenor_write, test_how_mocks_work)
{
    char buf[] = "hello world";
    mock().expectOneCall("write")
        .withParameter("fd", 1)
        .withOutputParameterReturning("buf", buf, sizeof buf)
        .withParameter("count", sizeof buf)
        .andReturnValue(666);

    CHECK_EQUAL(666, write(1, buf, sizeof buf));

    mock().checkExpectations();
}

#include <iostream>
TEST(chenor_write, test_how_mocks_work2)
{
    char buf[] = "hello world";

    mock().expectOneCall("write")
        .withParameter("fd", 1)
        .withOutputParameterReturning("buf", buf, sizeof buf)
        .withParameter("count", sizeof buf)
        .andReturnValue(666);

    CHECK_EQUAL(666, write(1, buf, sizeof buf));

    write_buf = static_cast<const std::vector<char>*>(
        mock().getData("buf").getObjectPointer());

    for (const auto &f: *write_buf)
        std::cout << f << std::endl;

    mock().checkExpectations();
}

int main(int argc, char **argv)
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
