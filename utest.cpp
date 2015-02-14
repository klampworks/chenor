#include "CppUTest/CommandLineTestRunner.h"

TEST_GROUP(first)
{};

TEST(first, test)
{
    FAIL("Start here");
}

int main(int argc, char **argv)
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
