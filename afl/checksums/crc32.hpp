/**
  *  \file afl/checksums/crc32.hpp
  *  \brief Class afl::checksums::CRC32
  */
#ifndef AFL_AFL_CHECKSUMS_CRC32_HPP
#define AFL_AFL_CHECKSUMS_CRC32_HPP

#include "afl/checksums/checksum.hpp"

namespace afl { namespace checksums {

    /** Cyclic redundancy check, 32 bits (CRC-32).
        This implements a big-endian CRC-32, as used in zlib, bzip2, and many others.
        It is characterized by a polynomial specified as a uint32_t. */
    class CRC32 : public Checksum {
     public:
        /** Constructor.
            Constructing the CRC32 object will compute a helper table and therefore is expensive.
            Re-use CRC32 objects if possible.
            Also see getDefaultInstance().

            \param poly Polynomial. The most significant bit is the x^0 term,
            the least-significant bit is the x^31 term, and there is an
            implicit x^32 term that is always present. */
        explicit CRC32(uint32_t poly);

        /** Destructor. */
        virtual ~CRC32();

        uint32_t add(Memory_t data, uint32_t prev) const;
        size_t bits() const;

        /** Get default instance.
            The default instance uses the zlib/bzip2 polynomial. */
        static CRC32& getDefaultInstance();

     protected:
        virtual uint32_t addImpl(Memory_t data, uint32_t prev) const;
        virtual size_t bitsImpl() const;

     private:
        /** Initialize. Helper function called by constructor.
            \param poly Polynomial */
        void init(uint32_t poly);

        /** Helper table. */
        uint32_t m_table[256];
    };

} }

#endif
