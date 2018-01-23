/**
  *  \file afl/test/socket.cpp
  *  \brief Class afl::test::Socket
  */

#include <algorithm>
#include "afl/test/socket.hpp"
#include "afl/async/controller.hpp"
#include "afl/async/notifier.hpp"
#include "afl/async/receiveoperation.hpp"
#include "afl/async/sendoperation.hpp"

inline
afl::test::Socket::Socket(const Assert& a, const afl::net::Name& name, afl::base::ConstBytes_t in, afl::base::ConstBytes_t out)
    : m_assert(a), m_name(name), m_in(in), m_out(out)
{ }

afl::base::Ref<afl::test::Socket>
afl::test::Socket::create(const Assert& a, const afl::net::Name& name, afl::base::ConstBytes_t in, afl::base::ConstBytes_t out)
{
    return *new Socket(a, name, in, out);
}

void
afl::test::Socket::closeSend()
{
    m_assert.check("Socket::closeSend", m_out.empty());
}

afl::net::Name
afl::test::Socket::getPeerName()
{
    return m_name;
}

bool
afl::test::Socket::send(afl::async::Controller& /*ctl*/, afl::async::SendOperation& op, afl::sys::Timeout_t /*timeout*/)
{
    size_t n = std::min(op.getUnsentBytes().size(), m_out.size());
    m_assert.checkEqualContent("Socket::send", op.getUnsentBytes().subrange(0, n), m_out.subrange(0, n));
    op.addSentBytes(n);
    m_out.split(n);
    return true;
}

void
afl::test::Socket::sendAsync(afl::async::Controller& ctl, afl::async::SendOperation& op)
{
    send(ctl, op, 0);
    op.getNotifier().notify(op);
}

bool
afl::test::Socket::receive(afl::async::Controller& /*ctl*/, afl::async::ReceiveOperation& op, afl::sys::Timeout_t /*timeout*/)
{
    size_t n = op.getUnreceivedBytes().copyFrom(m_in).size();
    op.addReceivedBytes(n);
    m_in.split(n);
    return true;
}

void
afl::test::Socket::receiveAsync(afl::async::Controller& ctl, afl::async::ReceiveOperation& op)
{
    receive(ctl, op, 0);
    op.getNotifier().notify(op);
}

void
afl::test::Socket::cancel(afl::async::Controller& ctl, afl::async::Operation& op)
{
    ctl.revertPost(op);
}

String_t
afl::test::Socket::getName()
{
    return m_name.toString();
}
