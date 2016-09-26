/**
  *  \file afl/sys/guard.hpp
  *  \brief Template class afl::sys::Guard
  */
#ifndef AFL_AFL_SYS_GUARD_HPP
#define AFL_AFL_SYS_GUARD_HPP

#include "afl/base/uncopyable.hpp"

namespace afl { namespace sys {

    /** Generic Guard.
        Implements exception-safe lock acquisition and release.
        \param T Lock type. A class implementing wait() and post() methods. */
    template<typename T>
    class Guard : afl::base::Uncopyable {
     public:
        /** Constructor.
            Acquires the lock.
            \param lockable Lock to acquire. */
        explicit Guard(T& lockable);

        /** Destructor.
            Releases the lock. */
        ~Guard();

     private:
        T& m_lockable;
    };

} }

template<typename T>
inline
afl::sys::Guard<T>::Guard(T& lockable)
    : m_lockable(lockable)
{
    m_lockable.wait();
}

template<typename T>
inline
afl::sys::Guard<T>::~Guard()
{
    m_lockable.post();
}

#endif
