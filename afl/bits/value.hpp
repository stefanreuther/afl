/**
  *  \file afl/bits/value.hpp
  *  \brief Template class afl::bits::Value
  */
#ifndef AFL_AFL_BITS_VALUE_HPP
#define AFL_AFL_BITS_VALUE_HPP

namespace afl { namespace bits {

    /** Packed value.
        Contains storage for a given type, and implicit packing/unpacking operators.

        This class is intended to build structures for a given on-disk layout,
        even if that on-disk layout does not match the host's endian or alignment constraints.
        For example, a structure containing a Value<Int16LE> and a Value<Int32LE> will have 6 bytes,
        whereas a structure containing a int16_t and a int32_t will most likely have 8 bytes.

        \param Desc Description of the field; see afl/bits/pack.hpp for details */
    template<typename Desc>
    struct Value {
        /** Assignment operator.
            \param word Value to pack and assign
            \return *this */
        Value& operator=(typename Desc::Word_t word);

        /** Conversion to underlying type.
            \return Unpacked value */
        operator typename Desc::Word_t() const;

        /** Explicit conversion to underlying type.
            \return Unpacked value */
        typename Desc::Word_t get() const;

        /** Representation. */
        typename Desc::Bytes_t m_bytes;
    };

} }

template<typename Desc>
inline afl::bits::Value<Desc>&
afl::bits::Value<Desc>::operator=(typename Desc::Word_t word)
{
    Desc::pack(m_bytes, word);
    return *this;
}

template<typename Desc>
inline
afl::bits::Value<Desc>::operator typename Desc::Word_t() const
{
    return Desc::unpack(m_bytes);
}

template<typename Desc>
inline typename Desc::Word_t
afl::bits::Value<Desc>::get() const
{
    return Desc::unpack(m_bytes);
}

#endif
