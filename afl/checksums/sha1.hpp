/**
  *  \file afl/checksums/sha1.hpp
  *  \brief Class afl::checksums::SHA1
  */
#ifndef AFL_AFL_CHECKSUMS_SHA1_HPP
#define AFL_AFL_CHECKSUMS_SHA1_HPP

#include "afl/checksums/hash.hpp"

namespace afl { namespace checksums {

    /** SHA-1 Hash.
        SHA-1 takes an arbitrary number of bytes and produces a 20-byte hash.

        Specification reference: RFC 3174 */
    class SHA1 : public Hash {
     public:
        /** Constructor. Makes a blank hash. */
        SHA1();

        /** Destructor. */
        ~SHA1();

        // Hash:
        virtual void clear();
        virtual void add(ConstBytes_t data);
        virtual size_t getHashSize() const;
        virtual size_t getBlockSize() const;
        virtual Bytes_t getHash(Bytes_t data) const;

        static const size_t HASH_SIZE = 20;

     private:
        typedef uint8_t Block_t[64];
        Block_t m_block;

        uint32_t m_state[HASH_SIZE/4];

        uint64_t m_bytes;

        void processBlock(const Block_t& block);
        void padMessage();
    };

} }

#endif
