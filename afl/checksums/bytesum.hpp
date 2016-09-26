/**
  *  \file afl/checksums/bytesum.hpp
  *  \brief Class afl::checksums::ByteSum
  */
#ifndef AFL_AFL_CHECKSUMS_BYTESUM_HPP
#define AFL_AFL_CHECKSUMS_BYTESUM_HPP

#include "afl/checksums/checksum.hpp"

namespace afl { namespace checksums {

    /** Bytewise sum.
        Implements a checksum that just adds all bytes in the data block. */
    class ByteSum : public Checksum {
     public:
        ByteSum();
        virtual ~ByteSum();

        uint32_t add(Memory_t data, uint32_t prev) const;
        size_t bits() const;

     protected:
        virtual uint32_t addImpl(Memory_t data, uint32_t prev) const;
        virtual size_t bitsImpl() const;
    };

} }

#endif
