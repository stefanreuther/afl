/**
  *  \file afl/sys/atomicinteger.hpp
  *  \brief Class afl::sys::AtomicInteger
  */
#ifndef AFL_AFL_SYS_ATOMICINTEGER_HPP
#define AFL_AFL_SYS_ATOMICINTEGER_HPP

#include "afl/base/types.hpp"

namespace afl { namespace sys {

    /** Atomically accessible integer.
        This provides an uint32_t and some basic operations on it,
        which can be accessed atomically in a multi-threaded, multi-core program.
        The idea is to use special hardware support if available,
        and fall back to operating system (mutex) otherwise.

        Operations include appropriate memory barriers to make sure that a modification
        to the AtomicInteger is seen by all cores.

        Note that although this class permits code like
        <code>
             ai = ai + 3;
        </code>
        to be written, that notation is <em>not</em> atomic with respect to the addition,
        because it suffers from the usual read/modify/write race.
        In contrast,
        <code>
             ai += 3;
        </code>
        <em>is</em> atomic. */
    class AtomicInteger {
     public:
        /** Constructor.
            Initialize with a constant value.
            \param n Value */
        AtomicInteger(uint32_t n);

        /** Assign new value.
            \param val New value
            \return *this */
        AtomicInteger& operator=(uint32_t val);

        /** Get value.
            \return value */
        operator uint32_t() const;

        /** Post-increment.
            Increments this value by one.
            It is guaranteed that with multiple threads calling this in parallel,
            each increment will be performed, and each thread will see a different result.
            \return new value */
        uint32_t operator++();

        /** Post-decrement.
            Decrements this value by one.
            It is guaranteed that with multiple threads calling this in parallel,
            each decrement will be performed, and each thread will see a different result.
            \return new value */
        uint32_t operator--();

        /** Add value.
            It is guaranteed that with multiple threads calling this in parallel,
            each addition will be performed, and each thread will see a different result
            (provided delta is nonzero of course).
            \param delta Value to add
            \return new value */
        uint32_t operator+=(uint32_t delta);

        /** Subtract value.
            It is guaranteed that with multiple threads calling this in parallel,
            each subtraction will be performed, and each thread will see a different result
            (provided delta is nonzero of course).
            \param delta Value to add
            \return new value */
        uint32_t operator-=(uint32_t delta);

        /** Replace if equal (compare-and-set).
            Use this as a fallback to implement operations that cannot be implemented using the built-in operations.
            For example,
            <code>
               uint32_t n;
               do {
                  n = ai;
               } while (!ai.replaceIfEqual(n, n | 0x80));
            </code>
            to implement a bit-set operation. */
        bool replaceIfEqual(uint32_t compare, uint32_t newVal);

     private:
        volatile uint32_t m_val;
    };

} }

inline
afl::sys::AtomicInteger::AtomicInteger(uint32_t n)
    : m_val(n)
{ }

inline uint32_t
afl::sys::AtomicInteger::operator++()
{
    return (*this) += 1;
}

inline uint32_t
afl::sys::AtomicInteger::operator--()
{
    return (*this) -= 1;
}

inline uint32_t
afl::sys::AtomicInteger::operator-=(uint32_t delta)
{
    return (*this) += -delta;
}

#endif
