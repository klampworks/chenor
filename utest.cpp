#include "CppUTest/CommandLineTestRunner.h"
#include <botan/auto_rng.h>
#include <botan/rng.h>
#include <botan/init.h>
#include <iostream>
#include <botan/rsa.h>
#include <botan/pkcs8.h>
#include "Fixed_Output_RNG.hpp"

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
    MK_FAKE_RNG_SIX(rng);

    CHECK_EQUAL(rng.next_byte(), rng.next_byte());
    CHECK_EQUAL(6, rng.next_byte());
}

template <typename K>
std::pair<std::string, std::string> serialise_key(const K &key)
{
    return {
        Botan::X509::PEM_encode(key), //Public Key
        Botan::PKCS8::PEM_encode(key) //Private Key
    };
}

#include <iostream>
void print_keys(const std::pair<std::string, std::string> &keys_s)
{
    std::cout << "################################################################################" << std::endl;
    std::cout << keys_s.first;
    std::cout << "################################################################################" << std::endl;

    std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;
    std::cout << keys_s.second;
    std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;
}

TEST(botan, create_key)
{
    Botan::LibraryInitializer init;
    MK_FAKE_RNG_INC(rng);

    std::string exp_pub_key(
"-----BEGIN PUBLIC KEY-----\n"
"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQC0VggrwctJPKaH4bUCzBHVFtgZ\n"
"3SLsOg1nSLKmJC3D55nbrhIIkrFlsJMOItIdBImtcNTags6/VZJ3BDscqeoRnyCx\n"
"kL01+ghf/+cUhz44dPKwreliFwcxlTEEDUu+ZDxFfud+QjJNkwGYVjpDcMEzx3tO\n"
"P013vRyVJc2LXkbTXwIDAQAB\n"
"-----END PUBLIC KEY-----\n"
);
    std::string exp_priv_key(
"-----BEGIN PRIVATE KEY-----\n"
"MIICdgIBADANBgkqhkiG9w0BAQEFAASCAmAwggJcAgEAAoGBALRWCCvBy0k8pofh\n"
"tQLMEdUW2BndIuw6DWdIsqYkLcPnmduuEgiSsWWwkw4i0h0Eia1w1NqCzr9VkncE\n"
"Oxyp6hGfILGQvTX6CF//5xSHPjh08rCt6WIXBzGVMQQNS75kPEV+535CMk2TAZhW\n"
"OkNwwTPHe04/TXe9HJUlzYteRtNfAgMBAAECgYAZq5l0KRz36uNFunJa2JO9Qog2\n"
"fk0HVWgtCKMtlT7SgThclhXJFKCd+hmkzH4eVVQH1WTnSfGGOPcla/vCI8nGxNIW\n"
"rpOUINWsc5vA3L3UvXHAGRi4x7Yf/yH5IJIcL2e/A6ZC0yWq/xvPiunmTpNQf/AR\n"
"gDYBUsZVz6RwLKiMIQJBAMABAgMEBQYHCAkKCwwNDg8QERITFBUWFxgZGhscHR4f\n"
"ICEiIyQlJicoKSorLC0uLzAxMjM0NTY3ODk6Ozw9PnECQQDwcXJzdHV2d3h5ent8\n"
"fX5/gIGCg4SFhoeIiYqLjI2Oj5CRkpOUlZaXmJmam5ydnp+goaKjpKWmp6ipqqus\n"
"rbbPAkAhOctsVCy+X0cfsVI6EqRFLQWXOB/4iisS630eBd5wEPjRYwPrxFX23rdI\n"
"6dGqO9zEnS7Pt5AhwqqDFLWddgexAkEA7wUH7mf+AOdg9vngWe/y2VLo69JL4eTL\n"
"RNrdxD3T1r02zM+2L8XIryi+waght7qhGrCzmhOprJMMoqWMBZumlwJAZHyiWJr+\n"
"GExJ7hoPM3Oj4aYBjgSqmlmsPISwc6rKAH5m7WGz9k51RNXYbCYJhmRv0Hw/ekmv\n"
"tPYBF5juSffiWg==\n"
"-----END PRIVATE KEY-----\n"
);
    auto private_key = Botan::RSA_PrivateKey(rng, 1024);
    auto keys_s = serialise_key(private_key);

    CHECK_EQUAL(exp_priv_key, keys_s.second);
}

int main(int argc, char **argv)
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
