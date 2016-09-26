/**
  *  \file arch/atomicinteger.hpp
  *  \brief System-dependant Part of afl/sys/atomicinteger.cpp
  */
#ifndef AFL_ARCH_ATOMICINTEGER_HPP
#define AFL_ARCH_ATOMICINTEGER_HPP

#include "afl/base/types.hpp"

#if defined(__GNUC__) && ((__GNUC__*100 + __GNUC_MINOR__) >= 407)
/*
 *  Implementation using GCC __atomic builtins (gcc 4.7 and later)
 */
namespace {
    inline void setAtomic(volatile uint32_t& var, uint32_t n)
    {
        __atomic_store_n(&var, n, __ATOMIC_SEQ_CST);
    }

    inline uint32_t getAtomic(const volatile uint32_t& var)
    {
        return __atomic_load_n(&var, __ATOMIC_SEQ_CST);
    }

    inline uint32_t incAtomic(volatile uint32_t& var, uint32_t n)
    {
        return __atomic_add_fetch(&var, n, __ATOMIC_SEQ_CST);
    }

    inline bool casAtomic(volatile uint32_t& var, uint32_t cmp, uint32_t set)
    {
        return __atomic_compare_exchange_n(&var, &cmp, set, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
    }
}

#elif defined(__i386) && defined(__GNUC__)
/*
 *  Implementation using i386 Instructions
 */
namespace {
    inline void barrier()
    {
        asm volatile("lock; addl $0, 0(%%esp)" : : : "memory");
    }

    inline void setAtomic(volatile uint32_t& var, uint32_t n)
    {
        barrier();              // Make sure previous stores are executed
        var = n;
        barrier();              // Make sure this store is executed
    }

    inline uint32_t getAtomic(const volatile uint32_t& var)
    {
        barrier();              // Make sure previous stores are executed
        return var;
    }

    inline uint32_t incAtomic(volatile uint32_t& var, uint32_t n)
    {
        uint32_t value = n;
        asm volatile("lock; xadd %0, (%1)" : "+r"(value) : "r"(&var) : "memory");
        return value + n;
    }

    inline bool casAtomic(volatile uint32_t& var, uint32_t cmp, uint32_t set)
    {
        uint8_t result;
        asm volatile("lock; cmpxchg %2, (%3)\n"
                     "setz %0" : "=r"(result), "+a"(cmp) : "r"(set), "r"(&var) : "memory");
        return result;
    }
}
#elif defined(TARGET_OS_POSIX)
/*
 *  POSIX Fallback Implementation using a Mutex
 */
# include <pthread.h>
namespace {
    pthread_mutex_t g_atomicMutex = PTHREAD_MUTEX_INITIALIZER;

    inline void setAtomic(volatile uint32_t& var, uint32_t n)
    {
        pthread_mutex_lock(&g_atomicMutex);
        var = n;
        pthread_mutex_unlock(&g_atomicMutex);
    }

    inline uint32_t getAtomic(const volatile uint32_t& var)
    {
        pthread_mutex_lock(&g_atomicMutex);
        uint32_t result = var;
        pthread_mutex_unlock(&g_atomicMutex);
        return result;
    }

    inline uint32_t incAtomic(volatile uint32_t& var, uint32_t n)
    {
        pthread_mutex_lock(&g_atomicMutex);
        uint32_t result = (var += n);
        pthread_mutex_unlock(&g_atomicMutex);
        return result;
    }

    inline bool casAtomic(volatile uint32_t& var, uint32_t cmp, uint32_t set)
    {
        pthread_mutex_lock(&g_atomicMutex);
        bool result;
        if (var == cmp) {
            var = set;
            result = true;
        } else {
            result = false;
        }
        pthread_mutex_unlock(&g_atomicMutex);
        return result;
    }
}
#else
# error Teach me about your atomic powers
#endif

#endif
