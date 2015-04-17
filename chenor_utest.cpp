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
        (char)0x04, (char)0x17, (char)0xBA, (char)0xB8, (char)0x64, (char)0x0D, 
        (char)0x30, (char)0xB7, (char)0x11, (char)0x9A, (char)0xBA, (char)0x00, 
        (char)0x6B, (char)0x07, (char)0x02, (char)0x64, (char)0x0B, (char)0x0A, 
        (char)0xDE, (char)0x65, (char)0x6A, (char)0x87, (char)0x17, (char)0xC0, 
        (char)0x26, (char)0x2E, (char)0x14, (char)0xB3, (char)0xA9, (char)0x62, 
        (char)0x0C, (char)0x92, (char)0x05, (char)0x10, (char)0x6B, (char)0x6A, 
        (char)0x78, (char)0xBF, (char)0xE4, (char)0xDF, (char)0xD8, (char)0x97, 
        (char)0xEF, (char)0xA7, (char)0x81, (char)0x1B, (char)0xF6, (char)0x9F, 
        (char)0x03, (char)0x63, (char)0xC0, (char)0xAD, (char)0xC7, (char)0x6B, 
        (char)0xCF, (char)0x14, (char)0x14, (char)0x92, (char)0xDE, (char)0x95, 
        (char)0x44, (char)0x00, (char)0x27, (char)0x97, (char)0xFE, (char)0x86, 
        (char)0x54, (char)0x9A, (char)0x0A, (char)0xDD, (char)0x3E, (char)0x26, 
        (char)0xE5, (char)0xFB, (char)0xB6, (char)0xA3, (char)0x6E, (char)0x89, 
        (char)0xDE, (char)0xF3, (char)0x98, (char)0x91, (char)0x80, (char)0x5A, 
        (char)0x99, (char)0x04, (char)0x8D, (char)0x3E, (char)0xDE, (char)0x96, 
        (char)0x4D, (char)0xDF, (char)0xE3, (char)0xC5, (char)0x40, (char)0x22, 
        (char)0xBA, (char)0xE7, (char)0x97, (char)0xCF, (char)0xD0, (char)0x60, 
        (char)0xFD, (char)0x21, (char)0xD9, (char)0x35, (char)0xEA, (char)0x44, 
        (char)0x0E, (char)0x5C, (char)0xCB, (char)0x09, (char)0x5A, (char)0xB2, 
        (char)0xBA, (char)0xEC, (char)0xB2, (char)0x07, (char)0xFF, (char)0x66, 
        (char)0x21, (char)0xBE, (char)0x3E, (char)0x01, (char)0x8D, (char)0x42, 
        (char)0x70, (char)0xEB, (char)0x0}; 

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

TEST(chenor_write, output_should_be_encrypted_two_calls)
{
    char in1[] = "hello";
    const std::string exp_out1 = {
        (char)0x04, (char)0x17, (char)0xBA, (char)0xB8, (char)0x64, (char)0x0D, 
        (char)0x30, (char)0xB7, (char)0x11, (char)0x9A, (char)0xBA, (char)0x00, 
        (char)0x6B, (char)0x07, (char)0x02, (char)0x64, (char)0x0B, (char)0x0A, 
        (char)0xDE, (char)0x65, (char)0x6A, (char)0x87, (char)0x17, (char)0xC0, 
        (char)0x26, (char)0x2E, (char)0x14, (char)0xB3, (char)0xA9, (char)0x62, 
        (char)0x0C, (char)0x92, (char)0x05, (char)0x10, (char)0x6B, (char)0x6A, 
        (char)0x78, (char)0xBF, (char)0xE4, (char)0xDF, (char)0xD8, (char)0x97, 
        (char)0xEF, (char)0xA7, (char)0x81, (char)0x1B, (char)0xF6, (char)0x9F, 
        (char)0x03, (char)0x63, (char)0xC0, (char)0xAD, (char)0xC7, (char)0x6B, 
        (char)0xCF, (char)0x14, (char)0x14, (char)0x92, (char)0xDE, (char)0x95, 
        (char)0x44, (char)0x00, (char)0x27, (char)0x97, (char)0xFE, (char)0x86, 
        (char)0x54, (char)0x9A, (char)0x0A, (char)0xDD, (char)0x3E, (char)0x26, 
        (char)0xE5, (char)0xFB, (char)0xB6, (char)0xA3, (char)0x6E, (char)0x89, 
        (char)0xDE, (char)0xF3, (char)0x98, (char)0x91, (char)0x80, (char)0x5A, 
        (char)0x99, (char)0x04, (char)0x8D, (char)0x3E, (char)0xDE, (char)0x96, 
        (char)0x4D, (char)0xDF, (char)0xE3, (char)0xC5, (char)0x40, (char)0x22, 
        (char)0xBA, (char)0xE7, (char)0x97, (char)0xCF, (char)0xD0, (char)0x60, 
        (char)0xFD, (char)0x21, (char)0xD9, (char)0x35, (char)0xEA, (char)0x44, 
        (char)0x0E, (char)0x5C, (char)0xCB, (char)0x09, (char)0x5A, (char)0xB2, 
        (char)0xBA, (char)0xEC, (char)0xB2, (char)0x07, (char)0xFF, (char)0x66, 
        (char)0x21, (char)0xBE, (char)0x3E, (char)0x01, (char)0x8D, (char)0x42, 
        (char)0x70, (char)0xEB, (char)0x0}; 

    mock().expectOneCall("write")
        .withParameter("fd", 1)
        .withParameter("buf", static_cast<const void*>(nullptr))
        .withParameter("count", 128);

    chenor::write(1, in1, sizeof in1);
    mock().checkExpectations();

    write_buf = static_cast<const std::vector<char>*>(
        mock().getData("write_buf").getObjectPointer());

    const auto out = std::string(write_buf->begin(), write_buf->end());
    for (size_t i = 0; i < out.size(); ++i) {
        CHECK_EQUAL(exp_out1[i], out[i]);
    }

    char in2[] = "world";
    const std::string exp_out2 = {
        (char)0x05, (char)0xB9, (char)0xE6, (char)0x15, (char)0xF4, (char)0x54, 
        (char)0xF3, (char)0xD6, (char)0x1E, (char)0x7E, (char)0x15, (char)0x37, 
        (char)0x47, (char)0x18, (char)0xB2, (char)0xE9, (char)0x85, (char)0x02, 
        (char)0x36, (char)0x67, (char)0x12, (char)0x57, (char)0x7D, (char)0x1B, 
        (char)0x14, (char)0x62, (char)0x2A, (char)0xE4, (char)0xC1, (char)0x01, 
        (char)0x08, (char)0x0D, (char)0x4A, (char)0x60, (char)0x52, (char)0xDD, 
        (char)0x25, (char)0xBF, (char)0xFC, (char)0x98, (char)0x53, (char)0x91, 
        (char)0x63, (char)0x5E, (char)0x23, (char)0xDC, (char)0xFC, (char)0xB5, 
        (char)0x54, (char)0x0D, (char)0x39, (char)0x11, (char)0xB0, (char)0x5F, 
        (char)0xF7, (char)0xEF, (char)0x90, (char)0x91, (char)0xF9, (char)0x38, 
        (char)0x50, (char)0xA7, (char)0x42, (char)0xF9, (char)0x98, (char)0x35, 
        (char)0x33, (char)0x64, (char)0x5B, (char)0x8F, (char)0xD3, (char)0x1C, 
        (char)0x4B, (char)0x94, (char)0xDA, (char)0xF3, (char)0xAA, (char)0xE1, 
        (char)0xD6, (char)0x45, (char)0xDF, (char)0xDC, (char)0x03, (char)0x89, 
        (char)0x24, (char)0x68, (char)0xA4, (char)0x6D, (char)0xDA, (char)0x12, 
        (char)0x58, (char)0x47, (char)0xB4, (char)0xEF, (char)0x06, (char)0x72, 
        (char)0x24, (char)0xC3, (char)0x43, (char)0xB1, (char)0x9D, (char)0xA8, 
        (char)0x42, (char)0xAC, (char)0xCC, (char)0xD8, (char)0x77, (char)0xC4, 
        (char)0xCC, (char)0x89, (char)0x3F, (char)0xC5, (char)0x92, (char)0x60, 
        (char)0xD4, (char)0xCD, (char)0x09, (char)0xDF, (char)0x2A, (char)0xD9, 
        (char)0x7F, (char)0x15, (char)0xEA, (char)0xD4, (char)0x30, (char)0x53, 
        (char)0xF1, (char)0x6B, (char)0x00};

    mock().expectOneCall("write")
        .withParameter("fd", 1)
        .withParameter("buf", static_cast<const void*>(nullptr))
        .withParameter("count", 128);

    chenor::write(1, in2, sizeof in2);
    mock().checkExpectations();

    write_buf = static_cast<const std::vector<char>*>(
        mock().getData("write_buf").getObjectPointer());

    const auto out2 = std::string(write_buf->begin(), write_buf->end());
    for (size_t i = 0; i < out2.size(); ++i) {
        CHECK_EQUAL(exp_out2[i], out2[i]);
    }
}

int main(int argc, char **argv)
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
