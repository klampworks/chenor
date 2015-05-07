#include <sys/types.h>
#include "write.hpp"
#include <botan/pubkey.h>
#include <botan/rsa.h>
#include <botan/types.h>
#include "chenor.hpp"
#include <botan/auto_rng.h>

using Botan::byte;

namespace chenor {

    Botan::RSA_PublicKey *public_key;
    std::unique_ptr<Botan::AutoSeeded_RNG> rng = nullptr;

    ssize_t write(int fd, const void *buf, size_t count)
    {
        const byte *st = static_cast<const byte*>(buf);
        const byte *en = st+count;
        Botan::PK_Encryptor_EME pke(*public_key, std::string("EME1(SHA-256)"));
        Botan::SecureVector<byte> enc;

        while (st < en) {

            auto d = (en - st);
            d = d > 62 ? 62 : d;
            enc += pke.encrypt(st, d, *chenor::rng);
            st += d;
        }

        return write_fp(fd, &enc[0], enc.size());
    }

    std::string decrypt(const std::vector<char> &in, Botan::RSA_PrivateKey *pk)
    {
        Botan::PK_Decryptor_EME pkd(*pk, std::string("EME1(SHA-256)"));
        auto st = (const byte*)(&in[0]);
        const auto en = st + in.size();
        std::string ret("");

        while (st < en) {
            const auto dec = pkd.decrypt(st, 128);
            ret += std::string(dec.begin(), dec.end());
            st += 128;
        }

        return ret;
    }

    void init()
    {

    }

    void init_rng()
    {
        //if (!rng)
            rng = std::unique_ptr<Botan::AutoSeeded_RNG>(
                new Botan::AutoSeeded_RNG);
    }

    void setup(Botan::RSA_PublicKey *pk)
    {
        init_rng();
        if (pk) {
            chenor::public_key = pk;
        } else {
            chenor::public_key = new Botan::RSA_PrivateKey(*chenor::rng, 1024);
        }
    }

    Botan::RSA_PrivateKey* gen_key()
    {
        init_rng();
        return new Botan::RSA_PrivateKey(*chenor::rng, 1024);
    }
}
