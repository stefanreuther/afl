/**
  *  \brief Class afl::checksums::MD5
  *  \file afl/checksums/md5.hpp
  */
#ifndef AFL_AFL_CHECKSUMS_MD5_HPP
#define AFL_AFL_CHECKSUMS_MD5_HPP

#include "afl/checksums/hash.hpp"

namespace afl { namespace checksums {

    /** MD5 hash.
        MD5 takes an arbitrary number of bytes and produces a 16-byte hash.

        Specification reference: RFC 1321. */
    class MD5 : public Hash {
     public:
        /** Constructor. Makes a blank hash. */
        MD5();

        /** Destructor. */
        ~MD5();

        // Hash:
        virtual void clear();
        virtual void add(ConstBytes_t data);
        virtual size_t getHashSize() const;
        virtual size_t getBlockSize() const;
        virtual Bytes_t getHash(Bytes_t data) const;

     private:
        uint64_t m_count;       ///< Number of bytes written (mod 2^64).

        typedef uint32_t State_t[4];
        State_t m_state;        ///< State (A/B/C/D).

        typedef uint8_t Block_t[64];
        Block_t m_buffer;       ///< Input reblocking buffer.

        static void transform(State_t& state, const Block_t& block);
    };

} }

#endif
