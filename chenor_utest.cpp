#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTestExt/MockSupport.h"
#include "chenor.hpp"
#include <vector>
#include "write.hpp"
#include <cstring>
#include <botan/init.h>

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

template <typename T, typename U>
void is_similar(const T in, size_t s, const U out)
{
    /* We basically just want to make sure that the input is not the same as the
     * output. There are bound to be occasional random simularities though. */
    size_t clashes = 0;
    for (size_t i = 0; i < s; ++i) {
        clashes += (in[i] == out[i]);
    }

    CHECK(clashes < (s/10)+1);
}

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
    Botan::LibraryInitializer init;
    chenor::setup();

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
    Botan::LibraryInitializer init;
    chenor::setup();
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
    Botan::LibraryInitializer init;
    chenor::setup();

    char in[] = "hello";

    mock().expectOneCall("write")
        .withParameter("fd", 1)
        .withParameter("buf", static_cast<const void*>(nullptr))
        .withParameter("count", 128);

    chenor::write(1, in, sizeof in);
    mock().checkExpectations();

    write_buf = static_cast<const std::vector<char>*>(
        mock().getData("write_buf").getObjectPointer());

    const auto out = std::string(write_buf->begin(), write_buf->end());
    for (size_t i = 0; i < strlen(in); ++i) {
        CHECK(in[i] != out[i]);
    }

    // Fails. WTF. I don't know.
    //CHECK_EQUAL(exp_out, out);
}

TEST(chenor_write, output_should_be_encrypted_two_calls)
{
    Botan::LibraryInitializer init;
    chenor::setup();

    char in1[] = "hello";

    mock().expectOneCall("write")
        .withParameter("fd", 1)
        .withParameter("buf", static_cast<const void*>(nullptr))
        .withParameter("count", 128);

    chenor::write(1, in1, sizeof in1);
    mock().checkExpectations();

    write_buf = static_cast<const std::vector<char>*>(
        mock().getData("write_buf").getObjectPointer());

    const auto out = std::string(write_buf->begin(), write_buf->end());
    is_similar(in1, strlen(in1), out);

    char in2[] = "world";

    mock().expectOneCall("write")
        .withParameter("fd", 1)
        .withParameter("buf", static_cast<const void*>(nullptr))
        .withParameter("count", 128);
    chenor::write(1, in2, sizeof in2);
    mock().checkExpectations();

    write_buf = static_cast<const std::vector<char>*>(
        mock().getData("write_buf").getObjectPointer());

    const auto out2 = std::string(write_buf->begin(), write_buf->end());
    is_similar(in2, strlen(in2), out2);
}

TEST(chenor_write, input_larger_than_128)
{
    Botan::LibraryInitializer init;
    chenor::setup();

    std::vector<char> in;
    for (int i = 0; i < 150; ++i)
        in.push_back(i%26+97);

    mock().expectOneCall("write")
        .withParameter("fd", 1)
        .withParameter("buf", static_cast<const void*>(nullptr))
        .withParameter("count", 384);

    chenor::write(1, &in[0], in.size());
    mock().checkExpectations();

    write_buf = static_cast<const std::vector<char>*>(
        mock().getData("write_buf").getObjectPointer());

    const auto out = std::string(write_buf->begin(), write_buf->end());
    is_similar(in, in.size(), out);
}
int main(int argc, char **argv)
{
    // Does not seem to take smark pointers into account. Disable it.
    MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
