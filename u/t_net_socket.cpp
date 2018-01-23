/**
  *  \file u/t_net_socket.cpp
  *  \brief Test for afl::net::Socket
  */

#include "afl/net/socket.hpp"

#include <cstdlib>
#include <cstdio>
#include "u/t_net.hpp"
#include "afl/async/controller.hpp"
#include "afl/async/receiveoperation.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/base/stoppable.hpp"
#include "afl/net/name.hpp"
#include "afl/net/networkstack.hpp"
#include "afl/sys/thread.hpp"

/** Simple socket test.
    This exercises the socket methods in a bidirectional connection. */
void
TestNetSocket::testBidi()
{
    // Make a network stack
    afl::net::NetworkStack& ns = afl::net::NetworkStack::getInstance();
    
    // Create a name with a random port number
    afl::net::Name name("127.0.0.1", uint16_t(std::rand() % 10000 + 20000));

    // Listen
    afl::base::Ref<afl::net::Listener> listener = ns.listen(name, 1);

    // Create a listening thread
    class Server : public afl::base::Stoppable {
     public:
        Server(afl::net::Listener& listener)
            : m_listener(listener)
            { }
        void run()
            {
                // Accept a single connection and echo the data back.
                afl::base::Ptr<afl::net::Socket> s = m_listener.accept();
                afl::async::Controller ctl;
                while (1) {
                    uint8_t buf[32];
                    afl::async::ReceiveOperation rx(buf);
                    s->receive(ctl, rx);
                    if (rx.getNumReceivedBytes() == 0) {
                        break;
                    }

                    afl::async::SendOperation tx(rx.getReceivedBytes());
                    s->send(ctl, tx);
                }
            }
        void stop()
            { }
     private:
        afl::net::Listener& m_listener;
    };
    Server server(*listener);
    afl::sys::Thread serverThread("serverThread", server);
    serverThread.start();

    // Connect to the server
    afl::base::Ref<afl::net::Socket> socket = ns.connect(name, 5000);
    TS_ASSERT(&socket.get() != 0);

    // Send some data
    static const uint8_t sendData[] = { 1, 2, 3 };
    afl::async::Controller ctl;
    afl::async::SendOperation tx(sendData);
    while (tx.getUnsentBytes().size() != 0) {
        tx.setData(tx.getUnsentBytes());
        socket->send(ctl, tx);
    }

    // Receive it back
    uint8_t receiveData[sizeof(sendData)];
    afl::async::ReceiveOperation rx(receiveData);
    while (rx.getUnreceivedBytes().size() != 0) {
        rx.setData(rx.getUnreceivedBytes());
        socket->receive(ctl, rx);
    }
    TS_ASSERT_SAME_DATA(sendData, receiveData, sizeof(sendData));

    // Shut down the writer. This will terminate the server thread.
    socket->closeSend();
    serverThread.join();
}

/** Test behaviour of socket where other end closes its side (SIGPIPE dangers lurk here). */
void
TestNetSocket::testClose()
{
    // Make a network stack
    afl::net::NetworkStack& ns = afl::net::NetworkStack::getInstance();
    
    // Create a name with a random port number
    afl::net::Name name("127.0.0.1", uint16_t(std::rand() % 10000 + 20000));

    // Listen
    afl::base::Ref<afl::net::Listener> listener = ns.listen(name, 1);

    // Create a listening thread
    class Server : public afl::base::Stoppable {
     public:
        Server(afl::net::Listener& listener)
            : m_listener(listener)
            { }
        void run()
            {
                // Accept a single connection.
                afl::base::Ptr<afl::net::Socket> s = m_listener.accept();

                // Receive 10 blocks, then stop the socket.
                // Windows sometimes rearranges the packet boundaries, so count bytes, not blocks.
                afl::async::Controller ctl;
                size_t count = 0;
                uint8_t buf[4096];
                while (count < 10*sizeof(buf)) {
                    afl::async::ReceiveOperation rx(buf);
                    s->receive(ctl, rx);
                    TS_ASSERT_DIFFERS(rx.getNumReceivedBytes(), 0U);
                    count += rx.getNumReceivedBytes();
                }
            }
        void stop()
            { }
     private:
        afl::net::Listener& m_listener;
    };
    Server server(*listener);
    afl::sys::Thread serverThread("serverThread", server);
    serverThread.start();

    // Connect to the server
    afl::base::Ref<afl::net::Socket> socket = ns.connect(name, 5000);
    TS_ASSERT(&socket.get() != 0);

    // Send 20 bytes. This will fail.
    static const uint8_t sendData[4096] = {42};
    afl::async::Controller ctl;
    size_t count = 0;
    for (int i = 0; i < 20; ++i) {
        try {
            afl::async::SendOperation tx(sendData);
            socket->send(ctl, tx);
            count += tx.getNumSentBytes();
        }
        catch (...) {
            TS_ASSERT(count >= 10U * sizeof(sendData));
        }
    }
    TS_ASSERT(count >= 10U * sizeof(sendData));

    // Shut down the writer. This will terminate the server thread.
    serverThread.join();
}
