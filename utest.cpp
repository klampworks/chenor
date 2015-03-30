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
#include <botan/b64_filt.h>
#include <botan/data_snk.h>
#include <botan/data_src.h>
#include <fstream>

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

void rm(const std::string &fn)
{
    system(std::string("rm " + fn).c_str());
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

TEST(botan, import_openssl_public_key_file)
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

TEST(botan, import_openssl_private_key_file)
{
    Botan::LibraryInitializer init;
    Botan::AutoSeeded_RNG rng;

    CHECK_THROWS(Botan::Decoding_Error,
        std::unique_ptr<Botan::Private_Key> /*priv_key*/ (
            Botan::PKCS8::load_key("openssl_rsa", rng))
    );
    #if 0
    const std::string exp_pub_key_s(
        "-----BEGIN PUBLIC KEY-----\n"
        "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDKd5VgqiGkfozIOccVT6UAo2/e\n"
        "IYzZ4AmzhW5R9m38SnRskyTFK3/vPGcXDpyE4WjFfeEOFqSFOTCHm9SvV0GMrION\n"
        "OR4s41qBTg1LpWp3iPmRPZMjIAZzk+TlnUO2yejyDDwapyE7Pg8AQBbiPEbO6dd4\n"
        "LL3rVs9fhswyOpqjpwIDAQAB\n"
        "-----END PUBLIC KEY-----\n"
    );

    const std::string exp_priv_key_s(
        "-----BEGIN RSA PRIVATE KEY-----\n"
        "MIICXQIBAAKBgQDKd5VgqiGkfozIOccVT6UAo2/eIYzZ4AmzhW5R9m38SnRskyTF\n"
        "K3/vPGcXDpyE4WjFfeEOFqSFOTCHm9SvV0GMrIONOR4s41qBTg1LpWp3iPmRPZMj\n"
        "IAZzk+TlnUO2yejyDDwapyE7Pg8AQBbiPEbO6dd4LL3rVs9fhswyOpqjpwIDAQAB\n"
        "AoGAZvlzK/rbqYIDsEJSkvAzB6QYXW3K6XUc+gU/GsivH8XueNpNtUpugwrBk+SG\n"
        "jr7mPqvLLxMtEeUIXbBP9+31E+N6Kc/pUGo7+Soz7Vwd27bjHgfDeNSAXs2i/bal\n"
        "mWkBUnWisBvJ8WVsjEWDb8B1CrC9bgD0tjAVGvWh0edVikECQQDn8XladVxs/7Ko\n"
        "peXmRuZM7/UfVm8seyZo2Urv2p0OBOsfbMpP7zJr6KS2Z2a2iI4NlsmaGQb5q024\n"
        "9E5horg/AkEA33d2j6axKS4U62jApoGq8UVi5J+PLaD3GkH8W7pjceSdZ9brV0WY\n"
        "Q1+UBLINqgTTmhvjNe/zWYVutvsRxFP6mQJBAKcFiyapppHTG3s4Y7IMUiELFo+n\n"
        "dU0prUIJX14TJc+HmSy68YDYNY3hLGMxWJSPBwQgFjLCXPSC9+f8/UhvnY8CQQCH\n"
        "Uum6TiCEpoJGmJXpfyaMmw8rEbD72bkp7oGq7rFf/CiSBGVZ4Sw2yH9zZHu/1NJr\n"
        "Ra4PMLK2KIaGSn5U3OIpAkAk/cwdHzBj4jR9Gjh9F02WO0OQdamXRbAcp9ti4PgU\n"
        "XJmALVFzbqn89oG+6ui82dNODWJZh08uJdwTomITgFu5\n"
        "-----END RSA PRIVATE KEY-----\n"
    );

    const std::string pub_key_s(Botan::X509::PEM_encode(*priv_key));
    const std::string priv_key_s(Botan::PKCS8::PEM_encode(*priv_key));
    
    CHECK_EQUAL(exp_pub_key_s, pub_key_s);
    CHECK_EQUAL(exp_priv_key_s, priv_key_s);
    #endif
}

TEST(botan, import_gpg_public_key_file)
{
    Botan::LibraryInitializer init;

    CHECK_THROWS(std::invalid_argument,
    std::unique_ptr<Botan::Public_Key> /*pub_key*/ (
        Botan::X509::load_key("gpg_rsa.pub")));

    #if 0
    const std::string exp_pub_key_s(
        "-----BEGIN PGP PUBLIC KEY BLOCK-----\n"
        "Version: GnuPG v2\n"
        "\n"
        "mI0EVgew6gEEANOXi7Cypo9NAL4v1cuPXLd2iIKPIEXh0iaocwd1kbNQzJykCgwy\n"
        "wT80+UdCNFMsMxEO6mYj7bywKPzsdCJ2VvORnx7WK97i6RpOmapg/1fTvw0hZeVY\n"
        "gqPRtt7bZHDHOBg7O8as1sfgTfRAhNugPxB905UQlRrPF9u4Rl5cGTZxABEBAAG0\n"
        "D2FhYWFhIDxhQGEuY29tPoi5BBMBAgAjBQJWB7DqAhsDBwsJCAcDAgEGFQgCCQoL\n"
        "BBYCAwECHgECF4AACgkQUPw3IYzirG7z6gP/ScsNVGQc5VoNajY91dIc3rkODRpk\n"
        "fAnXi5l3YNz5/+Q0utVouOhkdw3pMDxO1BubfeZwzs9rRRRKJLVkJeEK7+K4LRZd\n"
        "VApPL6RnvQOJGXyoT/vHnbLuGebkacq2BPxIfua4mZdF3Al7zJbbfLPzhIZ4C9uJ\n"
        "qZiy8HNJIsUc27u4jQRWB7DqAQQA5urP9ZJxUhACAuhiFK7QUQhIzcs6esWjOSlp\n"
        "+EYvMprABcegVAKUzOFFcjE1Jqqvt767OjR3uvMIfjBDBIEE15QBWfFHdBh/vES8\n"
        "Ljrwad7Zw0pvMWeLcSfiA6H9KTs7u5HzPVchu87/Y3jRWDIliZxbEKV0PW7OVeDb\n"
        "XesUtv8AEQEAAYifBBgBAgAJBQJWB7DqAhsMAAoJEFD8NyGM4qxuh2wD/0KHFcTM\n"
        "9sLKY1NwhYgFHrWsaIquYKqwKBO6GnWY4HyQ03xJXg5/BTlsgaawxj2ZokQH2mq4\n"
        "cA7s8gwfAVZWYfsBtJHdV1ZS2Mhz0EzDD+CWvYQvEiDjFZqL47KcRTRX28ACEOh3\n"
        "SO1yuirDKeV7CrY6vZDuNpoDTQ5OTXwK42CY\n"
        "=wowZ\n"
        "-----END PGP PUBLIC KEY BLOCK-----\n"
    );

    const std::string pub_key_s(Botan::X509::PEM_encode(*pub_key));
    CHECK_EQUAL(exp_pub_key_s, pub_key_s);
    #endif
}

TEST(botan, pipe_base64_encode)
{
    Botan::LibraryInitializer init;

    const std::string in("hello");
    const std::string exp_out("aGVsbG8=");

    Botan::Pipe pipe(new Botan::Base64_Encoder);
    pipe.start_msg();
    pipe.write(in);
    pipe.end_msg();

    const auto out = pipe.read_all_as_string(0);

    CHECK_EQUAL(exp_out, out);
}

TEST(botan, pipe_base64_decode)
{
    Botan::LibraryInitializer init;

    const std::string in("aGVsbG8=");
    const std::string exp_out("hello");

    Botan::Pipe pipe(new Botan::Base64_Decoder);
    pipe.start_msg();
    pipe.write(in);
    pipe.end_msg();

    const auto out = pipe.read_all_as_string(0);

    CHECK_EQUAL(exp_out, out);
}

std::string read_file(const std::string &fn)
{
    std::ifstream ifs(fn, std::ios::binary);
    std::string tmp;
    std::string ret;

    while(std::getline(ifs, tmp))
        ret += tmp;
        
    return ret;
}

TEST(botan, pipe_base64_encode_to_file)
{
    Botan::LibraryInitializer init;

    const std::string fn("pipe_base64_encode_to_file.txt");
    std::ofstream ofs(fn, std::ios::binary);
    const std::string in("hello");
    const std::string exp_out("aGVsbG8=");

    Botan::Pipe pipe(new Botan::Base64_Encoder, new Botan::DataSink_Stream(ofs));

    pipe.start_msg();
    pipe.write(in);
    pipe.end_msg();

    ofs.close();
    const std::string out = read_file(fn);
    CHECK_EQUAL(out, exp_out);
    rm(fn);
}

TEST(botan, pipe_base64_encode_to_file_decode_from_file)
{
    Botan::LibraryInitializer init;

    const std::string fn_enc("pipe_base64_encode_to_file.txt");
    const std::string fn_dec("pipe_base64_decode_from_file.txt");

    std::ofstream ofs(fn_enc, std::ios::binary);
    const std::string in("hello");

    Botan::Pipe pipe(new Botan::Base64_Encoder, new Botan::DataSink_Stream(ofs));
    pipe.process_msg(in);

    ofs.close();
    ofs.open(fn_dec, std::ios::binary);
    Botan::DataSource_Stream dss(fn_enc);

    Botan::Pipe pipe2(new Botan::Base64_Decoder, new Botan::DataSink_Stream(ofs));
    pipe2.process_msg(dss);

    ofs.close();

    const std::string out = read_file(fn_dec);
    CHECK_EQUAL(out, in);

    rm(fn_enc);
    rm(fn_dec);
}

TEST(botan, pipe_base64_encode_to_file_decode_from_file_multi_msg_fails)
{
    Botan::LibraryInitializer init;

    const std::string fn_enc("pipe_base64_encode_to_file_multi.txt");
    const std::string fn_dec("pipe_base64_decode_from_file_multi.txt");

    std::ofstream ofs(fn_enc, std::ios::binary);
    const std::string in1("hello");
    const std::string in2("world");

    Botan::Pipe pipe(new Botan::Base64_Encoder, new Botan::DataSink_Stream(ofs));
    pipe.process_msg(in1);
    pipe.process_msg(in2);

    ofs.close();
    ofs.open(fn_dec, std::ios::binary);
    Botan::DataSource_Stream dss(fn_enc);

    Botan::Pipe pipe2(new Botan::Base64_Decoder, new Botan::DataSink_Stream(ofs));
    pipe2.process_msg(dss);

    ofs.close();

    const std::string out = read_file(fn_dec);
    CHECK(out != in1 + in2);

    rm(fn_enc);
    rm(fn_dec);
}

TEST(botan, pipe_base64_encode_to_file_two_messages)
{
    Botan::LibraryInitializer init;

    const std::string fn("pipe_base64_encode_to_file_two_messages.txt");
    std::ofstream ofs(fn, std::ios::binary);
    const std::string in1("hello");
    const std::string in2("world");
    const std::string exp_out("aGVsbG8=d29ybGQ=");

    Botan::Pipe pipe(new Botan::Base64_Encoder, new Botan::DataSink_Stream(ofs));

    pipe.start_msg();
    pipe.write(in1);
    pipe.end_msg();

    pipe.start_msg();
    pipe.write(in2);
    pipe.end_msg();

    ofs.close();
    const std::string out = read_file(fn);
    CHECK_EQUAL(out, exp_out);
    rm(fn);
}


int main(int argc, char **argv)
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
