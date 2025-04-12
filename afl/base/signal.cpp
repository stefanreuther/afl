/**
  *  \file afl/base/signal.cpp
  *  \brief Template class afl::base::Signal
  */

#include "afl/base/signal.hpp"

afl::base::detail::SignalBaseBase::~SignalBaseBase()
{
    while (m_list) {
        delete m_list;
    }
}
