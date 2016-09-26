/**
  *  \file afl/sys/mutex.cpp
  *  \brief Class afl::sys::Mutex
  */

#include "afl/sys/mutex.hpp"
#include "arch/mutex.hpp"

afl::sys::Mutex::Mutex()
    : m_pImpl(new Impl())
{ }

afl::sys::Mutex::~Mutex()
{
    delete m_pImpl;
}

void
afl::sys::Mutex::wait()
{
    m_pImpl->wait();
}

void
afl::sys::Mutex::post()
{
    m_pImpl->post();
}
