#pragma once
#include <memory>
#include <string>
#include <vector>

#include <botan/rsa.h>
//#include <botan/auto_rng.h>

namespace Botan {
    class AutoSeeded_RNG;
}

namespace chenor {

    ssize_t write(int fd, const void *buf, size_t count);
    std::string decrypt(const std::vector<char> &in, Botan::RSA_PrivateKey *pk);

    // TODO, Find out why PublicKey destructors cause segfaults.
    extern Botan::RSA_PublicKey *public_key;
    //extern std::shared_ptr<Botan::AutoSeeded_RNG> rng;
    extern Botan::AutoSeeded_RNG *rng;
    Botan::RSA_PrivateKey* gen_key();

    void setup(Botan::RSA_PublicKey *pk=nullptr);
    void teardown();
}
