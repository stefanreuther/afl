/**
  *  \file afl/checksums/sha512.hpp
  *  \brief Class afl::checksums::SHA512
  */
#ifndef AFL_AFL_CHECKSUMS_SHA512_HPP
#define AFL_AFL_CHECKSUMS_SHA512_HPP

#include "afl/checksums/hash.hpp"
#include "afl/checksums/sha2core.hpp"

namespace afl { namespace checksums {

    /** SHA-512 Hash.
        SHA-512 takes an arbitrary number of bytes and produces a 64-byte hash.

        Specification reference: RFC 6234 */
    class SHA512 : public Hash {
     public:
        /** Constructor. Makes a blank hash. */
        SHA512();

        /** Destructor. */
        ~SHA512();

        virtual void clear();
        virtual void add(ConstBytes_t data);
        virtual size_t getHashSize() const;
        virtual size_t getBlockSize() const;
        virtual Bytes_t getHash(Bytes_t data) const;

        static const size_t HASH_SIZE = 64;   /* 512 bits = 8x 64 bits = 8x 8 bytes */

     private:
        SHA2Core<SHA2_64> m_state;
    };

} }

#endif
