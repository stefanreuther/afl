/**
  *  \file afl/test/networkstack.cpp
  *  \brief Class afl::test::NetworkStack
  */

#include <stdexcept>
#include "afl/test/networkstack.hpp"
#include "afl/test/socket.hpp"

afl::test::NetworkStack::NetworkStack(const Assert& a)
    : m_assert(a),
      m_connections()
{ }

afl::base::Ref<afl::net::Listener>
afl::test::NetworkStack::listen(const afl::net::Name& /*name*/, int /*backlogSize*/)
{
    m_assert.fail("NetworkStack::listen");
    throw std::runtime_error("unsupported");    // not reached
}

afl::base::Ref<afl::net::Socket>
afl::test::NetworkStack::connect(const afl::net::Name& name, afl::sys::Timeout_t /*timeout*/)
{
    // Must have a connection
    m_assert.check("NetworkStack::connect: must have connection", !m_connections.empty());

    // Consume that connection
    Connection c = m_connections.front();
    m_connections.pop_front();

    // Verify
    m_assert.checkEqual("NetworkStack::connect: name", name.getName(), c.name.getName());
    m_assert.checkEqual("NetworkStack::connect: service", name.getService(), c.name.getService());

    return Socket::create(m_assert, c.name, c.in, c.out);
}

void
afl::test::NetworkStack::expectConnection(const afl::net::Name& name, afl::base::ConstBytes_t in, afl::base::ConstBytes_t out)
{
    m_connections.push_back(Connection(name, in, out));
}
