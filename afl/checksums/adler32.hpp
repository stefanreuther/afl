/**
  *  \file afl/checksums/adler32.hpp
  *  \brief Class afl::checksums::Adler32
  */
#ifndef AFL_AFL_CHECKSUMS_ADLER32_HPP
#define AFL_AFL_CHECKSUMS_ADLER32_HPP

#include "afl/checksums/checksum.hpp"

namespace afl { namespace checksums {

    /** Adler-32 checksum.
        This implements the checksum as specified in RFC 1950 section 8.2.

        Note that unlike most other checksums, Adler32 should start at 1, not 0. */
    class Adler32 : public Checksum {
     public:
        Adler32();
        virtual ~Adler32();

        uint32_t add(Memory_t data, uint32_t prev) const;
        size_t bits() const;

     protected:
        virtual uint32_t addImpl(Memory_t data, uint32_t prev) const;
        virtual size_t bitsImpl() const;
    };

} }

#endif
