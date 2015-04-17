#include <sys/types.h>
#include "write.hpp"
#include <botan/pubkey.h>
#include "chenor.hpp"

namespace chenor {

    Botan::RSA_PrivateKey *private_key;
    Botan::AutoSeeded_RNG *rng;

    ssize_t write(int fd, void *buf, size_t count)
    {
        Botan::SecureVector<byte> xin;
        for (int i = 0; i < 10000; ++i)
            xin.push_back(i);

        //auto rng = new Fixed_Output_RNG(xin);
        //auto private_key = new Botan::RSA_PrivateKey(*chenor::rng, 1024);
        Botan::PK_Encryptor_EME pke(*private_key, std::string("EME1(SHA-256)"));
        auto enc = pke.encrypt((byte*)buf, count, *rng);
        //delete rng;
        //delete private_key;

        static char a[] = {
        (char)0x04, (char)0x17, (char)0xBA, (char)0xB8, (char)0x64, (char)0x0D, 
        (char)0x30, (char)0xB7, (char)0x11, (char)0x9A, (char)0xBA, (char)0x00, 
        (char)0x6B, (char)0x07, (char)0x02, (char)0x64, (char)0x0B, (char)0x0A, 
        (char)0xDE, (char)0x65, (char)0x6A, (char)0x87, (char)0x17, (char)0xC0, 
        (char)0x26, (char)0x2E, (char)0x14, (char)0xB3, (char)0xA9, (char)0x62, 
        (char)0x0C, (char)0x92, (char)0x05, (char)0x10, (char)0x6B, (char)0x6A, 
        (char)0x78, (char)0xBF, (char)0xE4, (char)0xDF, (char)0xD8, (char)0x97, 
        (char)0xEF, (char)0xA7, (char)0x81, (char)0x1B, (char)0xF6, (char)0x9F, 
        (char)0x03, (char)0x63, (char)0xC0, (char)0xAD, (char)0xC7, (char)0x6B, 
        (char)0xCF, (char)0x14, (char)0x14, (char)0x92, (char)0xDE, (char)0x95, 
        (char)0x44, (char)0x00, (char)0x27, (char)0x97, (char)0xFE, (char)0x86, 
        (char)0x54, (char)0x9A, (char)0x0A, (char)0xDD, (char)0x3E, (char)0x26, 
        (char)0xE5, (char)0xFB, (char)0xB6, (char)0xA3, (char)0x6E, (char)0x89, 
        (char)0xDE, (char)0xF3, (char)0x98, (char)0x91, (char)0x80, (char)0x5A, 
        (char)0x99, (char)0x04, (char)0x8D, (char)0x3E, (char)0xDE, (char)0x96, 
        (char)0x4D, (char)0xDF, (char)0xE3, (char)0xC5, (char)0x40, (char)0x22, 
        (char)0xBA, (char)0xE7, (char)0x97, (char)0xCF, (char)0xD0, (char)0x60, 
        (char)0xFD, (char)0x21, (char)0xD9, (char)0x35, (char)0xEA, (char)0x44, 
        (char)0x0E, (char)0x5C, (char)0xCB, (char)0x09, (char)0x5A, (char)0xB2, 
        (char)0xBA, (char)0xEC, (char)0xB2, (char)0x07, (char)0xFF, (char)0x66, 
        (char)0x21, (char)0xBE, (char)0x3E, (char)0x01, (char)0x8D, (char)0x42, 
        (char)0x70, (char)0xEB, (char)0x0}; 
        return write_fp(fd, &enc[0], 128 /*count*/);
    }

    void init()
    {

    }

    void setup()
    {
        chenor::rng = new Botan::AutoSeeded_RNG;
        chenor::private_key = new Botan::RSA_PrivateKey(*chenor::rng, 1024);
    }
}
