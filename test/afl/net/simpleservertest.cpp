/**
  *  \file test/afl/net/simpleservertest.cpp
  *  \brief Test for afl::net::SimpleServer
  */

#include "afl/net/simpleserver.hpp"

#include <cstdio>
#include "afl/async/controller.hpp"
#include "afl/async/receiveoperation.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/base/types.hpp"
#include "afl/net/name.hpp"
#include "afl/net/networkstack.hpp"
#include "afl/net/protocolhandler.hpp"
#include "afl/net/protocolhandlerfactory.hpp"
#include "afl/sys/thread.hpp"
#include "afl/test/testrunner.hpp"

namespace {
    // A protocol handler for testing.
    // - sending a packet containing "q" closes the connection
    // - everything else is echoed back
    class Handler : public afl::net::ProtocolHandler {
     public:
        Handler(afl::test::Assert a)
            : m_assert(a),
              m_buffer(),
              m_sent(true)
            { }

        virtual void getOperation(Operation& op)
            {
                if (!m_sent && m_buffer.size() > 0) {
                    if (m_buffer[0] == 'q') {
                        op.m_close = true;
                    } else {
                        op.m_dataToSend = afl::base::Bytes_t(m_buffer);
                    }
                }
                m_sent = true;
            }

        virtual void advanceTime(afl::sys::Timeout_t /*msecs*/)
            { }

        virtual void handleData(afl::base::ConstBytes_t bytes)
            {
                m_assert.check("handleData: m_sent", m_sent);
                m_assert.check("handleData: !bytes.empty", !bytes.empty());
                m_buffer.assign(bytes.unsafeData(), bytes.unsafeData() + bytes.size());
                m_sent = false;
            }

        virtual void handleSendTimeout(afl::base::ConstBytes_t /*unsentBytes*/)
            { }

        virtual void handleConnectionClose()
            { }

     private:
        afl::test::Assert m_assert;
        std::vector<uint8_t> m_buffer;
        bool m_sent;
    };

    class Factory : public afl::net::ProtocolHandlerFactory {
     public:
        Factory(afl::test::Assert a)
            : m_assert(a)
            { }
        Handler* create()
            {
                return new Handler(m_assert);
            }
     private:
        afl::test::Assert m_assert;
    };
}

/** Simple test. */
AFL_TEST("afl.net.SimpleServer", a)
{
    // Network stack
    afl::net::NetworkStack& stack = afl::net::NetworkStack::getInstance();
    afl::net::Name name("localhost", uint16_t(std::rand() % 30000 + 20000));

    Factory f("server");
    afl::net::SimpleServer server(stack.listen(name, 10), f);
    afl::sys::Thread thread("SimpleServer", server);
    thread.start();

    // Perform one stupid connection with no data transfer
    {
        afl::base::Ptr<afl::net::Socket> socket;
        AFL_CHECK_SUCCEEDS(a("01. connect"), socket = stack.connect(name, 500).asPtr());
        a.checkNonNull("02. socket", socket.get());
    }

    // Perform another connection
    {
        afl::base::Ptr<afl::net::Socket> socket;
        AFL_CHECK_SUCCEEDS(a("11. connect"), socket = stack.connect(name, 500).asPtr());
        a.check("12. socket", socket.get());

        // Async boilerplate
        afl::async::Controller ctl;
        afl::async::SendOperation tx;
        afl::async::ReceiveOperation rx;

        // Send something
        static const uint8_t send1[] = "hi";
        tx.setData(send1);
        a.check("13. send", socket->send(ctl, tx, 500));
        a.check("14. send complete", tx.isCompleted());

        // Receive something
        uint8_t receive1[10];
        rx.setData(receive1);
        a.check("14. receive", socket->receive(ctl, rx, 500));
        a.checkEqual("15. receive bytes", rx.getNumReceivedBytes(), sizeof(send1));
        a.check("16. receive content", rx.getReceivedBytes().equalContent(send1));

        // Request stop
        server.stop();

        // Send shutdown
        static const uint8_t send2[] = "q";
        tx.setData(send2);
        a.check("21. send", socket->send(ctl, tx, 500));
        a.check("22. send complete", tx.isCompleted());

        // Receive shutdown
        rx.setData(receive1);
        a.check("23. receive", socket->receive(ctl, rx, 500));
        a.checkEqual("24. receive complete", rx.getNumReceivedBytes(), 0U);
    }

    thread.join();
}
