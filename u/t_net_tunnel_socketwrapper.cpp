/**
  *  \file u/t_net_tunnel_socketwrapper.cpp
  *  \brief Test for afl::net::tunnel::SocketWrapper
  */

#include "afl/net/tunnel/socketwrapper.hpp"

#include "t_net_tunnel.hpp"
#include "afl/async/controller.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/async/receiveoperation.hpp"

namespace {
    class SocketMock : public afl::net::Socket {
     public:
        virtual void closeSend()
            { m_op += "closeSend,"; }
        virtual afl::net::Name getPeerName()
            { TS_ASSERT(0); return afl::net::Name(); }
        virtual bool send(afl::async::Controller& /*ctl*/, afl::async::SendOperation& /*op*/, afl::sys::Timeout_t /*timeout*/)
            { m_op += "send,"; return true; }
        virtual void sendAsync(afl::async::Controller& /*ctl*/, afl::async::SendOperation& /*op*/)
            { m_op += "sendAsync,"; }
        virtual bool receive(afl::async::Controller& /*ctl*/, afl::async::ReceiveOperation& /*op*/, afl::sys::Timeout_t /*timeout*/)
            { m_op += "receive,"; return true; }
        virtual void receiveAsync(afl::async::Controller& /*ctl*/, afl::async::ReceiveOperation& /*op*/)
            { m_op += "receiveAsync,"; }
        virtual void cancel(afl::async::Controller& /*ctl*/, afl::async::Operation& /*op*/)
            { m_op += "cancel,"; }
        virtual String_t getName()
            { TS_ASSERT(0); return String_t(); }
        const String_t& getResult() const
            { return m_op; }
     private:
        String_t m_op;
    };
}

void
TestNetTunnelSocketWrapper::testIt()
{
    afl::base::Ref<SocketMock> p(*new SocketMock());
    afl::net::tunnel::SocketWrapper testee(p, afl::net::Name("so", "123"), afl::net::Name("pe", "456"));

    // Test names
    TS_ASSERT_EQUALS(testee.getPeerName().toString(), "pe:456");
    TS_ASSERT_EQUALS(testee.getName(), "so:123");

    // Test method forwarding
    // - prepare
    TS_ASSERT_EQUALS(p->getResult(), "");
    afl::async::Controller ctl;
    afl::async::SendOperation sendOp;
    afl::async::ReceiveOperation receiveOp;

    // - do
    testee.send(ctl, sendOp, 1000);
    testee.receive(ctl, receiveOp, 1000);
    testee.sendAsync(ctl, sendOp);
    testee.receiveAsync(ctl, receiveOp);
    testee.cancel(ctl, sendOp);
    testee.cancel(ctl, receiveOp);

    // - verify
    TS_ASSERT_EQUALS(p->getResult(), "send,receive,sendAsync,receiveAsync,cancel,cancel,");
}

