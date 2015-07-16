#include <sys/types.h>
#include "write.hpp"
#include <botan/pubkey.h>
#include <botan/rsa.h>
#include <botan/types.h>
#include "chenor.hpp"
#include <botan/auto_rng.h>

using Botan::byte;

ssize_t chenor::write(int fd, const void *buf, size_t count)
{
    const static ssize_t ENCRYPT_BLOCK_MAX_SIZE = 62;
    const byte *st = static_cast<const byte*>(buf);
    const byte *en = st+count;
    Botan::SecureVector<byte> enc;

    while (st < en) {

        auto d = (en - st);
        d = d > ENCRYPT_BLOCK_MAX_SIZE ? ENCRYPT_BLOCK_MAX_SIZE : d;
        enc += pke->encrypt(st, d, *rng);
        st += d;
    }

    return write_fp(nullptr, fd, &enc[0], enc.size());
}

std::string chenor::decrypt(const std::vector<char> &in, Botan::RSA_PrivateKey *pk)
{
    const static size_t DECRYPT_BLOCK_MAX_SIZE = 128;
    Botan::PK_Decryptor_EME pkd(*pk, eme);
    auto st = (const byte*)(&in[0]);
    const auto en = st + in.size();
    std::string ret("");

    while (st < en) {
        const auto dec = pkd.decrypt(st, DECRYPT_BLOCK_MAX_SIZE);
        ret += std::string(dec.begin(), dec.end());
        st += DECRYPT_BLOCK_MAX_SIZE;
    }

    return ret;
}

chenor::chenor(Botan::RSA_PrivateKey *pk)
 : chenor(static_cast<Botan::RSA_PublicKey*>(pk))
{
}

chenor::chenor(Botan::RSA_PublicKey *pk)
    : eme("EME1(SHA-256)")
{
    rng = std::shared_ptr<Botan::AutoSeeded_RNG>(new Botan::AutoSeeded_RNG);

    if (pk) {
        chenor::public_key = std::shared_ptr<Botan::RSA_PublicKey>(pk);
    } else {
        chenor::public_key = std::shared_ptr<Botan::RSA_PublicKey>(
            new Botan::RSA_PrivateKey(*rng, 1024));
    }

    pke = std::shared_ptr<Botan::PK_Encryptor_EME>(
        new Botan::PK_Encryptor_EME(*public_key, eme));
}

Botan::RSA_PrivateKey* chenor::gen_key()
{
    Botan::AutoSeeded_RNG rng;
    return new Botan::RSA_PrivateKey(rng, 1024);
}

