/**
  *  \file afl/base/inlinememory.hpp
  *  \brief Template class afl::base::InlineMemory
  */
#ifndef AFL_AFL_BASE_INLINEMEMORY_HPP
#define AFL_AFL_BASE_INLINEMEMORY_HPP

#include "afl/base/memory.hpp"
#include "afl/base/types.hpp"

namespace afl { namespace base {

    /** Memory descriptor with preloaded memory.
        This is a utility class that works exactly like Memory.
        However, it comes preloaded with a buffer of N elements.
        For the common case of initializing a memory descriptor for a temporary buffer,
        this saves you having to declare (and invent a name for) the buffer variable.

        Otherwise, it behaves exactly like Memory.
        In particular, reset() clears it and does not initialize it to its initial state;
        it can also be made pointing at totally different memory.
        Copying does not copy the content, only the descriptor. */
    template<typename T, size_t N>
    class InlineMemory : public Memory<T> {
     public:
        /** Constructor. */
        InlineMemory();

        /** Copy operator. */
        InlineMemory& operator=(const Memory<T>& other);

     private:
        // Private copy operator. There is no point in copy-initializing an InlineMemory.
        template<typename U> InlineMemory(Memory<U> other);

        T m_data[N];
    };

} }

template<typename T, size_t N>
inline
afl::base::InlineMemory<T,N>::InlineMemory()
    : Memory<T>(m_data)
{ }

template<typename T, size_t N>
afl::base::InlineMemory<T,N>&
afl::base::InlineMemory<T,N>::operator=(const Memory<T>& other)
{
    Memory<T>::operator=(other);
    return *this;
}

#endif
