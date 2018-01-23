/**
  *  \file afl/checksums/sha256.hpp
  *  \brief Class afl::checksums::SHA256
  */
#ifndef AFL_AFL_CHECKSUMS_SHA256_HPP
#define AFL_AFL_CHECKSUMS_SHA256_HPP

#include "afl/checksums/hash.hpp"
#include "afl/checksums/sha2core.hpp"

namespace afl { namespace checksums {

    /** SHA-256 Hash.
        SHA-256 takes an arbitrary number of bytes and produces a 32-byte hash.

        Specification reference: RFC 6234 */
    class SHA256 : public Hash {
     public:
        /** Constructor. Makes a blank hash. */
        SHA256();

        /** Destructor. */
        ~SHA256();

        // Hash:
        virtual void clear();
        virtual void add(ConstBytes_t data);
        virtual size_t getHashSize() const;
        virtual size_t getBlockSize() const;
        virtual Bytes_t getHash(Bytes_t data) const;

        static const size_t HASH_SIZE = 32;   /* 256 bits = 8x 32 bits = 8x 4 bytes */

     private:
        SHA2Core<SHA2_32> m_state;
    };

} }

#endif
