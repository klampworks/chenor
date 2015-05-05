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
    std::string decrypt(const std::vector<char> &in);
    void init();

    // TODO, Find out why PrivateKey destructors cause segfaults.
    extern Botan::RSA_PrivateKey *private_key;
    extern std::unique_ptr<Botan::AutoSeeded_RNG> rng;

    void setup();
}
