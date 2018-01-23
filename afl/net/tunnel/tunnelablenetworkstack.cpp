/**
  *  \file afl/net/tunnel/tunnelablenetworkstack.cpp
  *  \brief Class afl::net::tunnel::TunnelableNetworkStack
  */

#include "afl/net/tunnel/tunnelablenetworkstack.hpp"
#include "afl/net/tunnel/socks4networkstack.hpp"
#include "afl/net/tunnel/socks5networkstack.hpp"

namespace {
    // Default port for SOCKS
    const char*const DEFAULT_PORT = "1080";
}

// Constructor.
afl::net::tunnel::TunnelableNetworkStack::TunnelableNetworkStack(NetworkStack& base)
    : m_base(base),
      m_current(&base),
      m_deleter()
{ }

// Destructor.
afl::net::tunnel::TunnelableNetworkStack::~TunnelableNetworkStack()
{ }

// Add tunnel (string).
bool
afl::net::tunnel::TunnelableNetworkStack::add(const String_t& link)
{
    Url u;
    return u.parse(link) && add(u);
}

// Add tunnel (URL).
bool
afl::net::tunnel::TunnelableNetworkStack::add(const Url& url)
{
    if (url.getScheme() == "socks4") {
        m_current = &m_deleter.addNew(new Socks4NetworkStack(*m_current, url.getName(DEFAULT_PORT)));
        return true;
    } else if (url.getScheme() == "socks5" || url.getScheme() == "socks") {
        m_current = &m_deleter.addNew(new Socks5NetworkStack(*m_current, url.getName(DEFAULT_PORT)));
        return true;
    } else {
        return false;
    }
}

// Reset.
void
afl::net::tunnel::TunnelableNetworkStack::reset()
{
    m_current = &m_base;
    m_deleter.clear();
}

afl::base::Ref<afl::net::Listener>
afl::net::tunnel::TunnelableNetworkStack::listen(const Name& name, int backlogSize)
{
    return m_current->listen(name, backlogSize);
}

afl::base::Ref<afl::net::Socket>
afl::net::tunnel::TunnelableNetworkStack::connect(const Name& name, afl::sys::Timeout_t timeout)
{
    return m_current->connect(name, timeout);
}
