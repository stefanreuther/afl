/**
  *  \file afl/checksums/sha384.hpp
  *  \brief Class afl::checksums::SHA384
  */
#ifndef AFL_AFL_CHECKSUMS_SHA384_HPP
#define AFL_AFL_CHECKSUMS_SHA384_HPP

#include "afl/checksums/hash.hpp"
#include "afl/checksums/sha2core.hpp"

namespace afl { namespace checksums {

    /** SHA-384 Hash.
        SHA-384 takes an arbitrary number of bytes and produces a 48-byte hash.

        Specification reference: RFC 6234 */
    class SHA384 : public Hash {
     public:
        /** Constructor. Makes a blank hash. */
        SHA384();

        /** Destructor. */
        ~SHA384();

        // Hash:
        virtual void clear();
        virtual void add(ConstBytes_t data);
        virtual size_t getHashSize() const;
        virtual size_t getBlockSize() const;
        virtual Bytes_t getHash(Bytes_t data) const;

        static const size_t HASH_SIZE = 48;   /* 384 bits = 6x 64 bits = 6x 8 bytes */

     private:
        SHA2Core<SHA2_64> m_state;
    };

} }

#endif
