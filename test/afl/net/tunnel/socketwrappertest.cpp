/**
  *  \file test/afl/net/tunnel/socketwrappertest.cpp
  *  \brief Test for afl::net::tunnel::SocketWrapper
  */

#include "afl/net/tunnel/socketwrapper.hpp"

#include <stdexcept>
#include "afl/async/controller.hpp"
#include "afl/async/receiveoperation.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/test/testrunner.hpp"

namespace {
    class SocketMock : public afl::net::Socket {
     public:
        virtual void closeSend()
            { m_op += "closeSend,"; }
        virtual afl::net::Name getPeerName()
            { throw std::runtime_error("getPeerName unexpected"); }
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
            { throw std::runtime_error("getName unexpected"); }
        const String_t& getResult() const
            { return m_op; }
     private:
        String_t m_op;
    };
}

AFL_TEST("afl.net.tunnel.SocketWrapper", a)
{
    afl::base::Ref<SocketMock> p(*new SocketMock());
    afl::net::tunnel::SocketWrapper testee(p, afl::net::Name("so", "123"), afl::net::Name("pe", "456"));

    // Test names
    a.checkEqual("getPeerName", testee.getPeerName().toString(), "pe:456");
    a.checkEqual("getName", testee.getName(), "so:123");

    // Test method forwarding
    // - prepare
    a.checkEqual("getResult initial", p->getResult(), "");
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
    a.checkEqual("getResult final", p->getResult(), "send,receive,sendAsync,receiveAsync,cancel,cancel,");
}
