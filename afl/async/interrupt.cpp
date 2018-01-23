/**
  *  \file afl/async/interrupt.cpp
  *  \brief Interface afl::async::Interrupt
  */

#include "afl/async/interrupt.hpp"
#include "arch/interrupt.hpp"

afl::async::Interrupt&
afl::async::Interrupt::getInstance()
{
    static Interrupt_t instance;
    return instance;
}
