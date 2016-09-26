/**
  *  \file afl/base/ptr.cpp
  *  \brief Template class afl::base::Ptr
  */

#include "afl/base/ptr.hpp"
#include "afl/sys/mutex.hpp"
#include "afl/container/ptrmap.hpp"
#include "afl/sys/mutexguard.hpp"

afl::sys::AtomicInteger*
afl::base::detail::getReferenceCount(void* p)
{
    static afl::sys::Mutex mtx;
    static afl::container::PtrMap<void*, afl::sys::AtomicInteger> data;

    afl::sys::MutexGuard g(mtx);
    afl::sys::AtomicInteger* result = data[p];
    if (!result) {
        result = new afl::sys::AtomicInteger(0);
        data.insertNew(p, result);
    }
    return result;
}

