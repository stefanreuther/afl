/**
  *  \file afl/checksums/sha224.hpp
  *  \brief Class afl::checksums::SHA224
  */
#ifndef AFL_AFL_CHECKSUMS_SHA224_HPP
#define AFL_AFL_CHECKSUMS_SHA224_HPP

#include "afl/checksums/hash.hpp"
#include "afl/checksums/sha2core.hpp"

namespace afl { namespace checksums {

    /** SHA-224 Hash.
        SHA-224 takes an arbitrary number of bytes and produces a 28-byte hash.

        Specification reference: RFC 6234 */
    class SHA224 : public Hash {
     public:
        /** Constructor. Makes a blank hash. */
        SHA224();

        /** Destructor. */
        ~SHA224();

        // Hash:
        virtual void clear();
        virtual void add(ConstBytes_t data);
        virtual size_t getHashSize() const;
        virtual size_t getBlockSize() const;
        virtual Bytes_t getHash(Bytes_t data) const;

        static const size_t HASH_SIZE = 28;   /* 224 bits = 7x 32 bits = 7x 4 bytes */

     private:
        SHA2Core<SHA2_32> m_state;
    };

} }

#endif
