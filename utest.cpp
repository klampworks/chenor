#include "CppUTest/CommandLineTestRunner.h"
#include <botan/auto_rng.h>
#include <botan/rng.h>
#include <botan/init.h>
#include <iostream>
#include <botan/rsa.h>
#include <botan/pkcs8.h>
#include <botan/pubkey.h>
#include "Fixed_Output_RNG.hpp"
#include <iostream>

#define MK_FAKE_RNG_INC(name) MK_FAKE_RNG(name, i)
#define MK_FAKE_RNG_SIX(name) MK_FAKE_RNG(name, 6)

#define MK_FAKE_RNG(name, n) \
    Botan::SecureVector<byte> in; \
    for (int i = 0; i < 10000; ++i) \
        in.push_back(n); \
 \
    Fixed_Output_RNG name(in);

void print_keys(const std::pair<std::string, std::string> &keys_s)
{
    std::cout << 
        "########################################" <<
        "########################################" << 
        keys_s.first <<
        "########################################" <<
        "########################################" << 
        std::endl;

    std::cout << 
        "<@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << 
        "<@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << 
        keys_s.second <<
        "<@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << 
        "<@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << 
        std::endl;
}

Botan::SecureVector<byte> str_to_secvec(const std::string &s)
{
    return Botan::SecureVector<byte>((byte*)s.c_str(), s.size());
}

const char* hello_cipher_hex()
{
    // The word "hello" encrypted with a MK_FAKE_RNG_INC and RSA "EME1(SHA-256)".
    // The ciphertext is huge because it is padded.
    return "0417BAB8640D30B7119ABA006B0702640B0ADE656A8717C0262E14B3A9620C925"
    "106B6A78BFE4DFD897EFA7811BF69F0363C0ADC76BCF141492DE9544002797FE86549A0A"
    "DD3E26E5FBB6A36E89DEF39891805A99048D3EDE964DDFE3C54022BAE797CFD060FD21D9"
    "35EA440E5CCB095AB2BAECB207FF6621BE3E018D4270EB0";
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

TEST(botan, fake_rng_gives_fixed_number)
{
    Botan::LibraryInitializer init;
    MK_FAKE_RNG_SIX(rng);

    CHECK_EQUAL(rng.next_byte(), rng.next_byte());
    CHECK_EQUAL(6, rng.next_byte());
}

TEST(botan, cannot_create_key_with_one_digit_rng)
{
    Botan::LibraryInitializer init;
    MK_FAKE_RNG_SIX(rng);

    CHECK_THROWS(Botan::Internal_Error, 
        Botan::RSA_PrivateKey(rng, 1024));
}


TEST(botan, create_key)
{
    Botan::LibraryInitializer init;
    MK_FAKE_RNG_INC(rng);

    std::string exp_pub_key_s(
"-----BEGIN PUBLIC KEY-----\n"
"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQC0VggrwctJPKaH4bUCzBHVFtgZ\n"
"3SLsOg1nSLKmJC3D55nbrhIIkrFlsJMOItIdBImtcNTags6/VZJ3BDscqeoRnyCx\n"
"kL01+ghf/+cUhz44dPKwreliFwcxlTEEDUu+ZDxFfud+QjJNkwGYVjpDcMEzx3tO\n"
"P013vRyVJc2LXkbTXwIDAQAB\n"
"-----END PUBLIC KEY-----\n"
);
    std::string exp_priv_key_s(
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
    Botan::RSA_PrivateKey private_key(rng, 1024);

    const std::string pub_key_s(Botan::X509::PEM_encode(private_key));
    const std::string priv_key_s(Botan::PKCS8::PEM_encode(private_key));

    CHECK_EQUAL(exp_priv_key_s, priv_key_s);
    CHECK_EQUAL(exp_pub_key_s, pub_key_s);
}

TEST(botan, encrypt_cstring)
{
    Botan::LibraryInitializer init;
    MK_FAKE_RNG_INC(rng);

    // Private Key class inherits from Public key class.
    Botan::RSA_PrivateKey private_key(rng, 1024);
    Botan::RSA_PublicKey *public_key = &private_key;

    Botan::PK_Encryptor_EME pke(*public_key, std::string("EME1(SHA-256)"));

    const char *plain = "hello";
    const char *exp_cipher_hex = hello_cipher_hex();

    const Botan::SecureVector<byte> exp_cipher = 
        Botan::hex_decode(exp_cipher_hex, strlen(exp_cipher_hex));

    const Botan::SecureVector<byte> cipher = 
        pke.encrypt((byte*)plain, strlen(plain), rng);

    const std::string cipher_hex = Botan::hex_encode(cipher, true);

    CHECK_EQUAL(cipher, exp_cipher);
    CHECK(strcmp(cipher_hex.c_str(), exp_cipher_hex) == 0);
}

TEST(botan, decrypt_cstring)
{
    Botan::LibraryInitializer init;
    MK_FAKE_RNG_INC(rng);

    Botan::RSA_PrivateKey private_key(rng, 1024);

    Botan::PK_Decryptor_EME pkd(private_key, std::string("EME1(SHA-256)"));

    const char *exp_plain_str = "hello";
    const char *cipher_hex = hello_cipher_hex();

    Botan::SecureVector<byte> cipher = 
        Botan::hex_decode(cipher_hex, strlen(cipher_hex), true);

    const Botan::SecureVector<byte> plain = pkd.decrypt(cipher);
    const char *plain_str = (char*)&(plain[0]);

    CHECK(strcmp(exp_plain_str, plain_str) == 0);
}

TEST(botan, encrypt_stdstring)
{
    Botan::LibraryInitializer init;
    MK_FAKE_RNG_INC(rng);

    const std::string exp_cipher_hex = hello_cipher_hex();

    const Botan::SecureVector<byte> exp_cipher = 
        Botan::hex_decode(exp_cipher_hex, true);

    const Botan::RSA_PrivateKey private_key(rng, 1024);
    const Botan::PK_Encryptor_EME pke(private_key, std::string("EME1(SHA-256)"));

    const std::string plain_s("hello");
    const Botan::SecureVector<byte> plain = str_to_secvec(plain_s);

    const Botan::SecureVector<byte> cipher = pke.encrypt(plain, rng);
    const std::string cipher_hex = Botan::hex_encode(cipher, true);

    CHECK_EQUAL(exp_cipher_hex, cipher_hex);
    CHECK_EQUAL(exp_cipher, cipher);
}

TEST(botan, decrypt_stdstring)
{
    Botan::LibraryInitializer init;
    MK_FAKE_RNG_INC(rng);

    const std::string exp_plain_s("hello");
    const Botan::SecureVector<byte> exp_plain = str_to_secvec(exp_plain_s);

    const std::string cipher_hex = hello_cipher_hex();

    const Botan::SecureVector<byte> cipher = 
        Botan::hex_decode(cipher_hex, true);

    const Botan::RSA_PrivateKey private_key(rng, 1024);
    const Botan::PK_Decryptor_EME pkd(private_key, std::string("EME1(SHA-256)"));

    const Botan::SecureVector<byte> plain = pkd.decrypt(cipher);
    const std::string plain_s(plain.begin(), plain.end());

    CHECK_EQUAL(exp_plain, plain);
    CHECK_EQUAL(exp_plain_s, exp_plain_s);
}

TEST(botan, import_openssl_key_file)
{
    Botan::LibraryInitializer init;
    std::unique_ptr<Botan::Public_Key> pub_key(
        Botan::X509::load_key("openssl_rsa.pub"));

    const std::string exp_pub_key_s(
        "-----BEGIN PUBLIC KEY-----\n"
        "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDKd5VgqiGkfozIOccVT6UAo2/e\n"
        "IYzZ4AmzhW5R9m38SnRskyTFK3/vPGcXDpyE4WjFfeEOFqSFOTCHm9SvV0GMrION\n"
        "OR4s41qBTg1LpWp3iPmRPZMjIAZzk+TlnUO2yejyDDwapyE7Pg8AQBbiPEbO6dd4\n"
        "LL3rVs9fhswyOpqjpwIDAQAB\n"
        "-----END PUBLIC KEY-----\n");

    const std::string pub_key_s(Botan::X509::PEM_encode(*pub_key));
    
    CHECK_EQUAL(exp_pub_key_s, pub_key_s);
}

int main(int argc, char **argv)
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
