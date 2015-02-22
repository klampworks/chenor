#include "CppUTest/CommandLineTestRunner.h"
#include <botan/auto_rng.h>
#include <botan/rng.h>
#include <botan/init.h>
#include <iostream>
#include <botan/rsa.h>
#include <botan/pkcs8.h>
#include "botan/Botan-1.10.5/checks/common.h"

namespace Botan {
    struct fake_rng : public RandomNumberGenerator {
        void randomize(byte bytes[], size_t len) { 
            for (size_t i = 0; i < len; ++i)
                bytes[i] = 6;
        }

        byte random() { return 6; }
        void clear() {}
        std::string name() const { return "fake_rng"; }

        byte next_byte() { return 6; };
        void reseed(size_t) {}
        bool is_seeded() const { return true; }
        void add_entropy(const byte[], size_t) {}
        void add_entropy_source(EntropySource* es) { delete es; }
    };
}

#define MK_FAKE_RNG_INC(name) MK_FAKE_RNG(name, i)
#define MK_FAKE_RNG_SIX(name) MK_FAKE_RNG(name, 6)

#define MK_FAKE_RNG(name, n) \
    Botan::SecureVector<byte> in; \
    for (int i = 0; i < 10000; ++i) \
        in.push_back(n); \
 \
    Fixed_Output_RNG name(in);

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

TEST(botan, fake_rng_gives_fixed_number)
{
    Botan::LibraryInitializer init;
    Botan::fake_rng rng;

    CHECK_EQUAL(rng.next_byte(), rng.next_byte());
    CHECK_EQUAL(6, rng.next_byte());
}

TEST(botan, create_key)
{
    Botan::LibraryInitializer init;
    //Botan::fake_rng rng;

    Botan::SecureVector<byte> in;
    for (int i = 0; i < 10000; ++i)
        in.push_back(i);

    Fixed_Output_RNG rng(in);
    //Botan::AutoSeeded_RNG rng;

    //terminate called after throwing an instance of 'Botan::Internal_Error'
    //  what():  Internal error: Assertion self_test_signature(encoded, plain_sig) failed (PK_Signer consistency check failed) in Botan::SecureVector<unsigned char> Botan::PK_Signer::signature(Botan::RandomNumberGenerator&) @src/pubkey/pubkey.cpp:218
    //  Aborted
    auto private_key = Botan::RSA_PrivateKey(rng, 1024);
    //auto public_key = Botan::X509::PEM_encode(private_key);


}

int main(int argc, char **argv)
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
