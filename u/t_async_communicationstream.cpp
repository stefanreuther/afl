/**
  *  \file u/t_async_communicationstream.cpp
  *  \brief Test for afl::async::CommunicationStream
  */

#include "afl/async/communicationstream.hpp"

#include <cstdio>
#include "u/t_async.hpp"
#include "afl/async/controller.hpp"
#include "afl/async/messageexchange.hpp"
#include "afl/base/stoppable.hpp"
#include "afl/sys/thread.hpp"
#include "afl/net/networkstack.hpp"
#include "afl/net/socket.hpp"
#include "afl/net/name.hpp"

namespace {
    /* Split the data amount in X*Y or Y*X to exercise reblocking. */
    const size_t X = 19;
    const size_t Y = 17;
    const uint8_t START = 99;
    
    /** Produce data. Produces X x Y bytes. */
    void testWriter(afl::base::Ptr<afl::async::CommunicationObject> comm)
    {
        // Async stuff
        afl::async::Controller ctl;
        afl::async::CommunicationStream cs(ctl, comm);

        // Produce the data
        uint8_t n = START;
        for (size_t i = 0; i < X; ++i) {
            // Set up buffer
            uint8_t buffer[Y];
            for (size_t j = 0; j < Y; ++j) {
                buffer[j] = n++;
            }

            // Write
            TS_ASSERT_THROWS_NOTHING(cs.fullWrite(buffer));
        }
    }

    /** Consume data. Consumes Y x X bytes. */
    void testReader(afl::base::Ptr<afl::async::CommunicationObject> comm)
    {
        // Async stuff
        afl::async::Controller ctl;
        afl::async::CommunicationStream cs(ctl, comm);

        // Produce the data
        uint8_t n = START;
        for (size_t i = 0; i < Y; ++i) {
            // Set up buffer and read
            uint8_t buffer[X];
            TS_ASSERT_THROWS_NOTHING(cs.fullRead(buffer));

            // Verify buffer
            for (size_t j = 0; j < X; ++j) {
                TS_ASSERT_EQUALS(buffer[j], n);
                ++n;
            }
        }
    }
}


/** Test using a MessageExchange. */
void
TestAsyncCommunicationStream::testMX()
{
    // Server half
    class Server : public afl::base::Stoppable {
     public:
        Server(afl::base::Ptr<afl::async::MessageExchange> pmx)
            : m_pmx(pmx)
            { }
        void run()
            { testWriter(m_pmx); }
        void stop()
            { }
     private:
        afl::base::Ptr<afl::async::MessageExchange> m_pmx;
    };


    // Create everything
    afl::base::Ptr<afl::async::MessageExchange> pmx = new afl::async::MessageExchange();
    Server server(pmx);
    afl::sys::Thread t("testMX", server);
    t.start();

    // Test
    testReader(pmx);

    // Finish
    t.join();
}

/** Test using a Socket. */
void
TestAsyncCommunicationStream::testSocket()
{
    // Server half, this time with sockets
    class Server : public afl::base::Stoppable {
     public:
        Server(afl::base::Ref<afl::net::Listener> pListener)
            : m_pListener(pListener)
            { }
        void run()
            {
                afl::base::Ptr<afl::net::Socket> socket = m_pListener->accept();
                testWriter(socket);
            }
        void stop()
            { }
     private:
        afl::base::Ref<afl::net::Listener> m_pListener;
    };

    // Build a network name with a random port number
    afl::net::Name name("127.0.0.1", uint16_t(std::rand() % 10000 + 20000));

    // Setup network
    afl::net::NetworkStack& ns = afl::net::NetworkStack::getInstance();
    afl::base::Ref<afl::net::Listener> listener = ns.listen(name, 1);

    // Setup background thread
    Server server(listener);
    afl::sys::Thread t("testSocket", server);
    t.start();

    // Connect and operate
    afl::base::Ref<afl::net::Socket> socket = ns.connect(name, 5000);
    TS_ASSERT(&socket.get() != 0);
    testReader(socket.asPtr());

    // Close
    t.join();
}

/** Test timeout behaviour. */
void
TestAsyncCommunicationStream::testTimeout()
{
    // This is pretty simplicistic.
    afl::base::Ptr<afl::async::MessageExchange> comm = new afl::async::MessageExchange();
    afl::async::Controller ctl;
    afl::async::CommunicationStream cs(ctl, comm);
    cs.setTimeout(100);

    // Try to read and write. Must all time out.
    uint8_t tmp[3];
    TS_ASSERT_EQUALS(cs.read(tmp), 0U);
    TS_ASSERT_EQUALS(cs.write(tmp), 0U);

    // Must still time out, previous operation is cancelled.
    TS_ASSERT_EQUALS(cs.read(tmp), 0U);
}
