/**
  *  \file afl/sys/mutexguard.hpp
  *  \brief Class afl::sys::MutexGuard
  */
#ifndef AFL_AFL_SYS_MUTEXGUARD_HPP
#define AFL_AFL_SYS_MUTEXGUARD_HPP

#include "afl/sys/guard.hpp"
#include "afl/sys/mutex.hpp"

namespace afl { namespace sys {

    /** Mutex Guard.
        Implements exception-safe mutex acquisition and release. */
    typedef Guard<Mutex> MutexGuard;

} }

#endif
