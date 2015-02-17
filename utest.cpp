#include "CppUTest/CommandLineTestRunner.h"
#include <botan/auto_rng.h>
#include <botan/init.h>
#include <iostream>

TEST_GROUP(first)
{};

TEST(first, test)
{
    Botan::LibraryInitializer init;
    Botan::AutoSeeded_RNG rng;
    std::cout << "hello world" << rng.next_byte() << std::endl;
    FAIL("Start here");
}

int main(int argc, char **argv)
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
