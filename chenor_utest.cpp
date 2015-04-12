#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTestExt/MockSupport.h"
#include "chenor.hpp"
#include <vector>
#include "write.hpp"
#include <cstring>

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
                mock().getData("write_buf").getObjectPointer());
        }

        delete write_buf;
        mock().clear();
    }
};

ssize_t mywrite(int fd, void *buf, size_t count)
{
    const auto cbuf = static_cast<char*>(buf);
    auto v = new std::vector<char>(cbuf, cbuf + count);
    mock().setDataObject("write_buf", "", v);

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

    CHECK_EQUAL(666, mywrite(1, buf, sizeof buf));

    mock().checkExpectations();
}

TEST(chenor_write, test_how_mocks_work2)
{
    char buf[] = "hello world";

    mock().expectOneCall("write")
        .withParameter("fd", 1)
        .withOutputParameterReturning("buf", buf, sizeof buf)
        .withParameter("count", sizeof buf)
        .andReturnValue(666);

    CHECK_EQUAL(666, mywrite(1, buf, sizeof buf));

    write_buf = static_cast<const std::vector<char>*>(
        mock().getData("write_buf").getObjectPointer());

    STRCMP_EQUAL(std::string(write_buf->begin(), write_buf->end()).c_str(), buf);
    mock().checkExpectations();
}

TEST(chenor_write, output_should_be_different_to_input)
{
    char in[] = "hello world";

    mock().expectOneCall("write")
        .withParameter("fd", 1)
        .withParameter("buf", static_cast<const void*>(nullptr))
        .withParameter("count", 128);

    chenor::write(1, in, sizeof in);
    mock().checkExpectations();

    write_buf = static_cast<const std::vector<char>*>(
        mock().getData("write_buf").getObjectPointer());

    CHECK(std::strcmp(
        std::string(write_buf->begin(), write_buf->end()).c_str(), 
        in) != 0);
}

TEST(chenor_write, output_should_be_at_least_128_bytes)
{
    char in[] = "hello world";

    CHECK(strlen(in) < 128);

    mock().expectOneCall("write")
        .withParameter("fd", 1)
        .withParameter("buf", static_cast<const void*>(nullptr))
        .withParameter("count", 128);

    chenor::write(1, in, sizeof in);
    mock().checkExpectations();

    write_buf = static_cast<const std::vector<char>*>(
        mock().getData("write_buf").getObjectPointer());

    CHECK(write_buf->size() >= 128);
}

int main(int argc, char **argv)
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
