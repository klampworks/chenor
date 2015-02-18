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

    auto prev = rng.next_byte();
    auto cur = prev;
    for (int i = 0; i < 100; ++i) {

        cur = rng.next_byte();

        if (cur != prev)
            break;

        prev = cur;
    }

    CHECK(cur != prev);
}

int main(int argc, char **argv)
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
