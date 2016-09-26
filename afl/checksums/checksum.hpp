/**
  *  \file afl/checksums/checksum.hpp
  *  \brief Interface afl::checksums::Checksum
  */
#ifndef AFL_AFL_CHECKSUMS_CHECKSUM_HPP
#define AFL_AFL_CHECKSUMS_CHECKSUM_HPP

#include "afl/base/deletable.hpp"
#include "afl/base/types.hpp"
#include "afl/base/memory.hpp"

namespace afl { namespace checksums {

    /** Checksum.
        A checksum is defined as an algorithm that computes a short (<= 32 bits) integer sum
        over an array of bytes to provide minimal validity checking.
        Checksums operate over a theoretically unbounded byte stream, and the split of this byte
        stream into blocks does not affect the computation result.
        A checksum has no internal state.

        The invariant is
        <code>
            add(wholeBlock, n) = add(secondHalf, add(firstHalf, n))
        </code>
        What follows from this invariant is that the checksum of an empty block is always 0.

        Everything that does not fit this description (larger output, larger state, not bytewise)
        is considered a hash.

        This class has two public methods, add() and bits(), that call their protected virtual counterparts.
        A derived class must override the virtual methods.
        It can do so by implementing the actual algorithm in its own public add() and bits() methods,
        and have the virtual function call those.
        This allows users with a concrete Checksum instance bypass the virtual dispatch. */
    class Checksum : public afl::base::Deletable {
     public:
        /** Type of a memory block. */
        typedef afl::base::ConstBytes_t Memory_t;

        /** Compute checksum.
            \param data Data block to compute checksum over.
            \param prev Previous checksum (initialize to 0).
            \return updated checksum */
        uint32_t add(Memory_t data, uint32_t prev) const;

        /** Get number of bits in checksum.
            \return Number of bits [1,32] */
        size_t bits() const;

     protected:
        /** Compute checksum, implementation.
            See class description and add(Memory_t,uint32_t).
            \param data Data block to compute checksum over.
            \param prev Previous checksum
            \return updated checksum */
        virtual uint32_t addImpl(Memory_t data, uint32_t prev) const = 0;

        /** Get number of bits in checksum, implementation.
            See class description and bits().
            \return Number of bits [1,32] */
        virtual size_t bitsImpl() const = 0;
    };

} }

inline uint32_t
afl::checksums::Checksum::add(Memory_t data, uint32_t prev) const
{
    return addImpl(data, prev);
}

inline size_t
afl::checksums::Checksum::bits() const
{
    return bitsImpl();
}

#endif
