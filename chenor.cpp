#include <sys/types.h>
#include "write.hpp"
#include <botan/pubkey.h>
#include "chenor.hpp"

namespace chenor {

    Botan::RSA_PrivateKey *private_key;
    std::unique_ptr<Botan::AutoSeeded_RNG> rng;

    ssize_t write(int fd, void *buf, size_t count)
    {
        Botan::PK_Encryptor_EME pke(*private_key, std::string("EME1(SHA-256)"));
        auto enc = pke.encrypt((byte*)buf, count, *rng);

        return write_fp(fd, &enc[0], 128 /*count*/);
    }

    void init()
    {

    }

    void setup()
    {
        chenor::rng = std::unique_ptr<Botan::AutoSeeded_RNG>(
            new Botan::AutoSeeded_RNG);
        chenor::private_key = new Botan::RSA_PrivateKey(*chenor::rng, 1024);
    }
}
