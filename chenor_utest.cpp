#include "CppUTest/CommandLineTestRunner.h"

TEST_GROUP(chenor_write)
{
};

TEST(chenor_write, start_here)
{
    FAIL("start_here");
}

int main(int argc, char **argv)
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
