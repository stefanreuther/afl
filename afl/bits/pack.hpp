/**
  *  \file afl/bits/pack.hpp
  *  \brief Packing and Unpacking
  *
  *  The functions in this file convert a real data type into a byte array and back.
  *
  *  They work on element descriptors.
  *  An element descriptor is a type with the following members:
  *  - a Word_t typedef: the data type
  *  - a Bytes_t typedef: defines an array of sufficient size to store an element of Word_t
  *  - a pack(Bytes_t&,Word_t) function that packs an element of Word_t into a byte array
  *  - an unpack(Bytes_t&) function that unpacks an array and returns an element of Word_t
  *
  *  See afl::bits::UInt16LE for a sample.
  */
#ifndef AFL_AFL_BITS_PACK_HPP
#define AFL_AFL_BITS_PACK_HPP

#include "afl/base/memory.hpp"

namespace afl { namespace bits {

    /** Unpack an array from a byte buffer.
        The template parameter T is an element descriptor.
        See the description of afl/bits/pack.hpp for details.
 
        \param T array element descriptor
        \param out [out] data array
        \param in [in] byte array */
    template<typename T>
    void
    unpackArray(afl::base::Memory<typename T::Word_t> out, afl::base::ConstBytes_t in)
    {
        typedef typename T::Word_t Word_t;
        typedef typename T::Bytes_t Bytes_t;
        while (Word_t* destEle = out.eat()) {
            if (const Bytes_t* srcEle = in.template eatN<sizeof(Bytes_t)>()) {
                *destEle = T::unpack(*srcEle);
            } else {
                *destEle = Word_t();
            }
        }
    }

    /** Pack an array into a byte buffer.
        The template parameter T is an element descriptor.
        See the description of afl/bits/pack.hpp for details.
 
        \param T array element descriptor
        \param out [out] byte array
        \param in [in] data array */
    template<typename T>
    void
    packArray(afl::base::Bytes_t out, afl::base::Memory<const typename T::Word_t> in)
    {
        typedef typename T::Word_t Word_t;
        typedef typename T::Bytes_t Bytes_t;
        while (Bytes_t* destEle = out.template eatN<sizeof(Bytes_t)>()) {
            if (const Word_t* srcEle = in.eat()) {
                T::pack(*destEle, *srcEle);
            } else {
                T::pack(*destEle, Word_t());
            }
        }

        // Output might end in a partial element. Fill that to not leave it uninitialized.
        out.fill(0);
    }

} }

#endif
