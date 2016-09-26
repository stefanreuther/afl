/**
  *  \file afl/net/networkstack.cpp
  *  \brief Interface afl::net::NetworkStack
  */

#include "afl/net/networkstack.hpp"
#include "arch/networkstack.hpp"

afl::net::NetworkStack&
afl::net::NetworkStack::getInstance()
{
    static NetworkStack_t instance;
    return instance;
}
