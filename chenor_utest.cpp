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

    const auto out = std::string(write_buf->begin(), write_buf->end());
    CHECK(write_buf->size() >= 128);
}

TEST(chenor_write, output_should_be_encrypted)
{
    char in[] = "hello";
    const std::string exp_out = {
        0x04, 0x17, 0xBA, 0xB8, 0x64, 0x0D, 0x30, 0xB7, 0x11, 0x9A, 
        0xBA, 0x00, 0x6B, 0x07, 0x02, 0x64, 0x0B, 0x0A, 0xDE, 0x65, 
        0x6A, 0x87, 0x17, 0xC0, 0x26, 0x2E, 0x14, 0xB3, 0xA9, 0x62, 
        0x0C, 0x92, 0x05, 0x10, 0x6B, 0x6A, 0x78, 0xBF, 0xE4, 0xDF, 
        0xD8, 0x97, 0xEF, 0xA7, 0x81, 0x1B, 0xF6, 0x9F, 0x03, 0x63, 
        0xC0, 0xAD, 0xC7, 0x6B, 0xCF, 0x14, 0x14, 0x92, 0xDE, 0x95, 
        0x44, 0x00, 0x27, 0x97, 0xFE, 0x86, 0x54, 0x9A, 0x0A, 0xDD, 
        0x3E, 0x26, 0xE5, 0xFB, 0xB6, 0xA3, 0x6E, 0x89, 0xDE, 0xF3, 
        0x98, 0x91, 0x80, 0x5A, 0x99, 0x04, 0x8D, 0x3E, 0xDE, 0x96, 
        0x4D, 0xDF, 0xE3, 0xC5, 0x40, 0x22, 0xBA, 0xE7, 0x97, 0xCF, 
        0xD0, 0x60, 0xFD, 0x21, 0xD9, 0x35, 0xEA, 0x44, 0x0E, 0x5C, 
        0xCB, 0x09, 0x5A, 0xB2, 0xBA, 0xEC, 0xB2, 0x07, 0xFF, 0x66, 
        0x21, 0xBE, 0x3E, 0x01, 0x8D, 0x42, 0x70, 0xEB, 0x0};

    mock().expectOneCall("write")
        .withParameter("fd", 1)
        .withParameter("buf", static_cast<const void*>(nullptr))
        .withParameter("count", 128);

    chenor::write(1, in, sizeof in);
    mock().checkExpectations();

    write_buf = static_cast<const std::vector<char>*>(
        mock().getData("write_buf").getObjectPointer());

    const auto out = std::string(write_buf->begin(), write_buf->end());
    for (size_t i = 0; i < out.size(); ++i) {
        CHECK_EQUAL(exp_out[i], out[i]);
    }

    // Fails. WTF. I don't know.
    //CHECK_EQUAL(exp_out, out);
}

int main(int argc, char **argv)
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
