/**
  *  \file afl/net/tunnel/socketwrapper.cpp
  */

#include "afl/net/tunnel/socketwrapper.hpp"

afl::net::tunnel::SocketWrapper::SocketWrapper(afl::base::Ref<Socket> base, const Name& socketName, const Name& peerName)
    : m_base(base),
      m_socketName(socketName),
      m_peerName(peerName)
{ }

afl::net::tunnel::SocketWrapper::~SocketWrapper()
{ }

// Socket
void
afl::net::tunnel::SocketWrapper::closeSend()
{
    m_base->closeSend();
}

afl::net::Name
afl::net::tunnel::SocketWrapper::getPeerName()
{
    return m_peerName;
}

// Socket
void
afl::net::tunnel::SocketWrapper::cancel(afl::async::Controller& ctl, afl::async::Operation& op)
{
    m_base->cancel(ctl, op);
}

bool
afl::net::tunnel::SocketWrapper::send(afl::async::Controller& ctl, afl::async::SendOperation& op, afl::sys::Timeout_t timeout)
{
    return m_base->send(ctl, op, timeout);
}

void
afl::net::tunnel::SocketWrapper::sendAsync(afl::async::Controller& ctl, afl::async::SendOperation& op)
{
    m_base->sendAsync(ctl, op);
}

bool
afl::net::tunnel::SocketWrapper::receive(afl::async::Controller& ctl, afl::async::ReceiveOperation& op, afl::sys::Timeout_t timeout)
{
    return m_base->receive(ctl, op, timeout);
}

void
afl::net::tunnel::SocketWrapper::receiveAsync(afl::async::Controller& ctl, afl::async::ReceiveOperation& op)
{
    m_base->receiveAsync(ctl, op);
}

String_t
afl::net::tunnel::SocketWrapper::getName()
{
    return m_socketName.toString();
}
