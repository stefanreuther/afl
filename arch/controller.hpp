/**
  *  \file arch/controller.hpp
  *  \brief System-dependant Part of afl/async/controller.cpp
  *
  *  This provides a typedef ControllerImpl_t referring to a class with (at least) four methods:
  *  - prepare() to prepare that we will soon call wait() after examining the result queue
  *  - wait(Timeout_t) to wait for an event, i.e. someone posting a result
  *  - wake() to signal from another thread that an event is posted
  *  - finish() to signal that we will soon leave the waiting function
  *
  *  prepare/finish permit optimisations to allow operating without expensive inter-thread signalisation
  *  when the thread owning the Controller isn't actually waiting.
  */
#ifndef AFL_ARCH_CONTROLLER_HPP
#define AFL_ARCH_CONTROLLER_HPP

#include "afl/async/controller.hpp"

#if TARGET_OS_POSIX
/*
 *  POSIX
 */
#include "arch/posix/posixcontrollerimpl.hpp"
class afl::async::Controller::Impl : public arch::posix::PosixControllerImpl { };
#elif TARGET_OS_WIN32
/*
 *  Win32
 */
#include "arch/win32/win32controllerimpl.hpp"
class afl::async::Controller::Impl : public arch::win32::Win32ControllerImpl { };
#else
/*
 *  Other
 */
#include "afl/sys/semaphore.hpp"
class afl::async::Controller::Impl {
 public:
    Impl()
        : m_sem(0)
        { }
    void wait(afl::sys::Timeout_t t)
        {
            m_sem.wait(t);
        }
    void wake()
        {
            m_sem.post();
        }
    void prepare()
        { }
    void finish()
        { }
    afl::sys::Semaphore m_sem;
};
#endif

#endif
