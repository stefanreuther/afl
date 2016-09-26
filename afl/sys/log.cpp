/**
  *  \file afl/sys/log.cpp
  *  \brief Class afl::sys::Log
  */

#include "afl/sys/log.hpp"
#include "afl/sys/mutex.hpp"
#include "afl/sys/mutexguard.hpp"

namespace {
    /*
     *  Locking is notoriously difficult here.
     *
     *  We cannot just put a mutex in LogListener and one in Log,
     *  and lock whichever object we enter first (Log most of the time,
     *  but LogListener for the destructor), because that would give a
     *  deadlock.
     *
     *  Likewise, we cannot just declare that Log always is locked first,
     *  because the Log might have died in the very instant LogListener
     *  decides to use it.
     *
     *  Therefore, use the simple brute-force approach: one big fat
     *  global lock. Assumptions are:
     *  - log messages are comparatively rare. Even 500 mutexes/second
     *    won't kill us, but two threads spewing into each other's private
     *    Log instance will still step on each other's feet, just because
     *    there could be a potential common LogListener.
     *  - addListener/removeListener are very rare.
     *  - all involved data structures are small (a handful of listeners
     *    and parents), so we can use ultra simple algorithms, vectors
     *    instead of hash-maps etc.
     */

    afl::sys::Mutex& getMutex()
    {
        static afl::sys::Mutex instance;
        return instance;
    }

    template<typename T>
    void removeOne(std::vector<T>& vec, T val)
    {
        for (typename std::vector<T>::iterator i = vec.begin(), e = vec.end(); i != e; ++i) {
            if (*i == val) {
                vec.erase(i);
                break;
            }
        }
    }

    template<typename T>
    void removeAll(std::vector<T>& vec, T val)
    {
        for (typename std::vector<T>::iterator i = vec.begin(); i != vec.end(); /* no increment */) {
            if (*i == val) {
                i = vec.erase(i);
            } else {
                ++i;
            }
        }
    }
}

afl::sys::Log::Log()
    : m_listeners()
{ }

afl::sys::Log::~Log()
{
    killParent(*this);
}

void
afl::sys::Log::handleMessage(const Message& msg)
{
    afl::sys::MutexGuard g(getMutex());
    for (size_t i = 0; i < m_listeners.size(); ++i) {
        m_listeners[i]->handleMessage(msg);
    }
}

afl::sys::Log&
afl::sys::Log::getDefaultInstance()
{
    static Log instance;
    return instance;
}

void
afl::sys::Log::addListener(LogListener& listener)
{
    afl::sys::MutexGuard g(getMutex());
    m_listeners.push_back(&listener);
    listener.m_parents.push_back(this);
}

void
afl::sys::Log::removeListener(LogListener& listener)
{
    afl::sys::MutexGuard g(getMutex());
    removeOne(m_listeners, &listener);
    removeOne(listener.m_parents, this);
}

void
afl::sys::Log::killListener(LogListener& listener)
{
    afl::sys::MutexGuard g(getMutex());

    // %listener dies, so remove all instances of %listener from all its parents.
    std::vector<Log*>& parents = listener.m_parents;
    for (std::vector<Log*>::iterator i = parents.begin(), e = parents.end(); i != e; ++i) {
        removeAll((*i)->m_listeners, &listener);
    }
    parents.clear();
}

void
afl::sys::Log::killParent(Log& parent)
{
    afl::sys::MutexGuard g(getMutex());

    // %parent dies, so remove all instance of %parent from all its listeners.
    std::vector<LogListener*>& listeners = parent.m_listeners;
    for (std::vector<LogListener*>::iterator i = listeners.begin(), e = listeners.end(); i != e; ++i) {
        removeAll((*i)->m_parents, &parent);
    }
    listeners.clear();
}
