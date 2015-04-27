#include <sys/types.h>
#include "write.hpp"
#include <botan/pubkey.h>
#include "chenor.hpp"

namespace chenor {

    Botan::RSA_PrivateKey *private_key;
    std::unique_ptr<Botan::AutoSeeded_RNG> rng;

    ssize_t write(int fd, void *buf, size_t count)
    {
        const byte *st = static_cast<byte*>(buf);
        const byte *en = st+count;
        Botan::PK_Encryptor_EME pke(*private_key, std::string("EME1(SHA-256)"));
        Botan::SecureVector<byte> enc;

        while (st < en) {
            const auto d = en - st;

            if (d > 62) {
                enc += pke.encrypt(st, 62, *chenor::rng);
                st += 62;
            } else {
                enc += pke.encrypt(st, d, *chenor::rng);
                st += d;
            }
        }

        return write_fp(fd, &enc[0], enc.size() /*count*/);
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
