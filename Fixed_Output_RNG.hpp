#pragma once

#include <vector>
#include <string>
#include <deque>
#include <stdexcept>

#include <botan/rng.h>
#include <botan/hex.h>

using Botan::byte;

class Fixed_Output_RNG : public Botan::RandomNumberGenerator
   {
   public:
      bool is_seeded() const { return !buf.empty(); }

      byte random()
         {
         if(buf.empty())
            throw std::runtime_error("Out of bytes");

         byte out = buf.front();
         buf.pop_front();
         return out;
         }

      void reseed(size_t) {}

      void randomize(byte out[], size_t len)
         {
         for(size_t j = 0; j != len; j++)
            out[j] = random();
         }

      std::string name() const { return "Fixed_Output_RNG"; }

      void add_entropy_source(Botan::EntropySource* src) { delete src; }
      void add_entropy(const byte[], size_t) {};

      void clear() throw() {}

      Fixed_Output_RNG(const Botan::SecureVector<byte>& in)
         {
         buf.insert(buf.end(), in.begin(), in.end());
         }
      Fixed_Output_RNG(const std::string& in_str)
         {
         Botan::SecureVector<byte> in = Botan::hex_decode(in_str);
         buf.insert(buf.end(), in.begin(), in.end());
         }

      Fixed_Output_RNG() {}
   private:
      std::deque<byte> buf;
   };

