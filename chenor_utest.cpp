#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTestExt/MockSupport.h"
#include "chenor.hpp"
#include <vector>
//#include <unistd.h>

TEST_GROUP(chenor_write)
{
    void teardown()
    {
        mock().clear();
    }
};

ssize_t write(int fd, void *buf, size_t count)
{
    mock().setData("test", 10);

    auto v = new std::vector<char>();
    for (int i = 0; i < 100; ++i)
        v->push_back('f');

    mock().setDataObject("test2", "", v);

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

    auto v = (const std::vector<char>*)mock().getData("test2").getObjectPointer();
    delete(v);

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

    int i = mock().getData("test").getIntValue();
    auto v = (const std::vector<char>*)mock().getData("test2").getObjectPointer();

    for (const auto &f: *v)
        std::cout << f << std::endl;

    delete(v);
    mock().checkExpectations();
}

int main(int argc, char **argv)
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
