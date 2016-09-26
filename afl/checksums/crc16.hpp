/**
  *  \file afl/checksums/crc16.hpp
  *  \brief Class afl::checksums::CRC16
  */
#ifndef AFL_AFL_CHECKSUMS_CRC16_HPP
#define AFL_AFL_CHECKSUMS_CRC16_HPP

#include "afl/checksums/checksum.hpp"

namespace afl { namespace checksums {

    /** Cyclic redundancy check, 16 bits (CRC-16).
        This implements a big-endian CRC-16.
        It is characterized by a polynomial specified as a uint16_t. */
    class CRC16 : public Checksum {
     public:
        /** Constructor.
            Constructing the CRC16 object will compute a helper table and therefore is expensive.
            Re-use CRC16 objects if possible.
            Also see getDefaultInstance().

            \param poly Polynomial. The most significant bit is the x^0 term,
            the least-significant bit is the x^15 term, and there is an
            implicit x^16 term that is always present. */
        explicit CRC16(uint16_t poly);

        /** Destructor. */
        virtual ~CRC16();

        uint16_t add(Memory_t data, uint16_t prev) const;
        size_t bits() const;

        /** Get default instance.
            The default instance uses the CCITT polynomial. */
        static CRC16& getDefaultInstance();

     protected:
        virtual uint32_t addImpl(Memory_t data, uint32_t prev) const;
        virtual size_t bitsImpl() const;

     private:
        /** Initialize. Helper function called by constructor.
            \param poly Polynomial */
        void init(uint16_t poly);

        /** Helper table. */
        uint16_t m_table[256];
    };

} }

#endif
