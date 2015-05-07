#pragma once
#include <memory>
#include <string>
#include <vector>

namespace Botan {
    class RSA_PrivateKey;
    class AutoSeeded_RNG;
}

namespace chenor {

    ssize_t write(int fd, const void *buf, size_t count);
    std::string decrypt(const std::vector<char> &in, Botan::RSA_PrivateKey *pk);
    void init();

    // TODO, Find out why PrivateKey destructors cause segfaults.
    extern Botan::RSA_PrivateKey *private_key;
    extern std::unique_ptr<Botan::AutoSeeded_RNG> rng;
    Botan::RSA_PrivateKey* gen_key();

    void setup(Botan::RSA_PrivateKey *pk=nullptr);
}
