/**
  *  \file test/afl/async/communicationsinktest.cpp
  *  \brief Test for afl::async::CommunicationSink
  *
  *  (This test is very similar to the one for CommunicationStream, and uses CommunicationStream.)
  */

#include "afl/async/communicationsink.hpp"
#include "afl/test/testrunner.hpp"

#include <stdexcept>
#include "afl/async/communicationstream.hpp"
#include "afl/async/controller.hpp"
#include "afl/async/messageexchange.hpp"
#include "afl/base/ptr.hpp"
#include "afl/base/stoppable.hpp"
#include "afl/net/listener.hpp"
#include "afl/net/name.hpp"
#include "afl/net/networkstack.hpp"
#include "afl/net/socket.hpp"
#include "afl/sys/thread.hpp"

namespace {
    /* Split the data amount in X*Y or Y*X to exercise reblocking. */
    const size_t X = 19;
    const size_t Y = 17;
    const uint8_t START = 99;

    /** Produce data. Produces X x Y bytes. */
    void testWriter(afl::test::Assert a, afl::base::Ptr<afl::async::CommunicationObject> comm)
    {
        // Async stuff
        afl::async::Controller ctl;
        afl::async::CommunicationSink cs(ctl, comm);

        // Produce the data
        uint8_t n = START;
        for (size_t i = 0; i < X; ++i) {
            // Set up buffer
            uint8_t buffer[Y];
            for (size_t j = 0; j < Y; ++j) {
                buffer[j] = n++;
            }

            // Write
            afl::base::ConstBytes_t bytes(buffer);
            a.check("handleData", !cs.handleData(bytes));
            a.check("empty", bytes.empty());
        }
    }

    /** Consume data. Consumes Y x X bytes. Uses CommunicationStream for reblocking. */
    void testReader(afl::test::Assert a, afl::base::Ptr<afl::async::CommunicationObject> comm)
    {
        // Async stuff
        afl::async::Controller ctl;
        afl::async::CommunicationStream cs(ctl, comm);

        // Produce the data
        uint8_t n = START;
        for (size_t i = 0; i < Y; ++i) {
            // Set up buffer and read
            uint8_t buffer[X];
            AFL_CHECK_SUCCEEDS(a("fullRead"), cs.fullRead(buffer));

            // Verify buffer
            for (size_t j = 0; j < X; ++j) {
                a.checkEqual("buffer content", buffer[j], n);
                ++n;
            }
        }
    }
}

/** Test using a MessageExchange. */
AFL_TEST("afl.async.CommunicationSink:MX", a)
{
    // Server half
    class Server : public afl::base::Stoppable {
     public:
        Server(afl::test::Assert a, afl::base::Ptr<afl::async::MessageExchange> pmx)
            : m_assert(a), m_pmx(pmx)
            { }
        void run()
            { testReader(m_assert, m_pmx); }
        void stop()
            { }
     private:
        afl::test::Assert m_assert;
        afl::base::Ptr<afl::async::MessageExchange> m_pmx;
    };


    // Create everything
    afl::base::Ptr<afl::async::MessageExchange> pmx = new afl::async::MessageExchange();
    Server server(a("reader"), pmx);
    afl::sys::Thread t("testMX", server);
    t.start();

    // Test
    testWriter(a("writer"), pmx);

    // Finish
    t.join();
}

/** Test using a Socket. */
AFL_TEST("afl.async.CommunicationSink:Socket", a)
{
    // Server half, this time with sockets
    class Server : public afl::base::Stoppable {
     public:
        Server(afl::test::Assert a, afl::base::Ref<afl::net::Listener> pListener)
            : m_assert(a), m_pListener(pListener)
            { }
        void run()
            {
                afl::base::Ptr<afl::net::Socket> socket = m_pListener->accept();
                testReader(m_assert, socket);
            }
        void stop()
            { }
     private:
        afl::test::Assert m_assert;
        afl::base::Ref<afl::net::Listener> m_pListener;
    };

    // Build a network name with a random port number
    afl::net::Name name("127.0.0.1", uint16_t(std::rand() % 10000 + 20000));

    // Setup network
    afl::net::NetworkStack& ns = afl::net::NetworkStack::getInstance();
    afl::base::Ref<afl::net::Listener> listener = ns.listen(name, 1);

    // Setup background thread
    Server server(a("reader"), listener);
    afl::sys::Thread t("testSocket", server);
    t.start();

    // Connect and operate
    afl::base::Ptr<afl::net::Socket> socket = ns.connect(name, 5000).asPtr();
    a.check("socket connected", socket.get() != 0);
    testWriter(a("writer"), socket);

    // Close
    t.join();
}

/** Test timeout behaviour. */
AFL_TEST("afl.async.CommunicationSink:timeout", a)
{
    // This is pretty simplicistic.
    afl::base::Ptr<afl::async::MessageExchange> comm = new afl::async::MessageExchange();
    afl::async::Controller ctl;
    afl::async::CommunicationSink cs(ctl, comm);
    cs.setTimeout(100);

    // Try to send. Must time out with an exception.
    uint8_t tmp[] = {1,2,3};
    afl::base::ConstBytes_t bytes(tmp);
    AFL_CHECK_THROWS(a("handleData 1"), cs.handleData(bytes), std::runtime_error);
    AFL_CHECK_THROWS(a("handleData 2"), cs.handleData(bytes), std::runtime_error);
}
