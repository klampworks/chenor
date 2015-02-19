#include "CppUTest/CommandLineTestRunner.h"
#include <botan/auto_rng.h>
#include <botan/rng.h>
#include <botan/init.h>
#include <iostream>

namespace Botan {
    struct fake_rng : public RandomNumberGenerator {
        void randomize(byte[], size_t) {  }
        void clear() {}
        std::string name() const { return "Null_RNG"; }

        byte next_byte() { return 6; };
        void reseed(size_t) {}
        bool is_seeded() const { return false; }
        void add_entropy(const byte[], size_t) {}
        void add_entropy_source(EntropySource* es) { delete es; }
    };
}

TEST_GROUP(botan)
{
};

TEST(botan, rng_gives_different_numbers)
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
