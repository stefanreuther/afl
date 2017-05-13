/**
  *  \file afl/net/nullnetworkstack.cpp
  *  \brief Class afl::net::NullNetworkStack
  */

#include "afl/net/nullnetworkstack.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/net/name.hpp"
#include "afl/string/messages.hpp"

afl::net::NullNetworkStack::NullNetworkStack()
    : NetworkStack()
{ }

afl::net::NullNetworkStack::~NullNetworkStack()
{ }

afl::base::Ref<afl::net::Listener>
afl::net::NullNetworkStack::listen(const Name& name, int /*backlogSize*/)
{
    throw afl::except::FileProblemException(name.toString(), afl::string::Messages::invalidOperation());
}

afl::base::Ref<afl::net::Socket>
afl::net::NullNetworkStack::connect(const Name& name, afl::sys::Timeout_t /*timeout*/)
{
    throw afl::except::FileProblemException(name.toString(), afl::string::Messages::invalidOperation());
}
