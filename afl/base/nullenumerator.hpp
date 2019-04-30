/**
  *  \file afl/base/nullenumerator.hpp
  *  \brief Template class afl::base::NullEnumerator
  */
#ifndef AFL_AFL_BASE_NULLENUMERATOR_HPP
#define AFL_AFL_BASE_NULLENUMERATOR_HPP

#include "afl/base/enumerator.hpp"

namespace afl { namespace base {

    /** Null enumerator.
        Returns an empty sequence */
    template<typename T>
    class NullEnumerator : public Enumerator<T> {
     public:
        // Enumerator:
        virtual ~NullEnumerator();
        virtual bool getNextElement(T& result);
    };

} }

template<typename T>
inline
afl::base::NullEnumerator<T>::~NullEnumerator()
{ }

template<typename T>
bool
afl::base::NullEnumerator<T>::getNextElement(T&)
{
    return false;
}

#endif
