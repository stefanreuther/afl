/**
  *  \file u/t_net_resp_client.cpp
  *  \brief Test for afl::net::resp::Client
  */

#include <stdio.h>
#include "afl/net/resp/client.hpp"

#include <cstdlib>
#include "u/t_net_resp.hpp"
#include "afl/async/communicationstream.hpp"
#include "afl/async/controller.hpp"
#include "afl/base/runnable.hpp"
#include "afl/net/listener.hpp"
#include "afl/net/name.hpp"
#include "afl/net/networkstack.hpp"
#include "afl/net/socket.hpp"
#include "afl/sys/semaphore.hpp"
#include "afl/sys/thread.hpp"
#include "afl/data/integervalue.hpp"
#include "afl/except/remoteerrorexception.hpp"
#include "afl/base/ref.hpp"

/** Simple test. */
void
TestNetRespClient::testIt()
{
    using afl::base::Ref;
    class Tester : public afl::base::Runnable {
     public:
        Tester(Ref<afl::net::Listener> listener)
            : m_listener(listener),
              m_in(),
              m_out(),
              m_wake(0)
            { }

        void run()
            {
                // Accept one connection and wrap it in a communication stream
                afl::async::Controller ctl;
                afl::async::CommunicationStream io(ctl, m_listener->accept());

                // Read expected data
                while (1) {
                    // Wait for caller to provide data
                    m_wake.wait();
                    if (m_in.empty()) {
                        break;
                    }

                    // Wait for data
                    uint8_t buffer[1024];
                    TS_ASSERT_LESS_THAN_EQUALS(m_in.size(), sizeof(buffer));
                    TS_ASSERT_THROWS_NOTHING(io.fullRead(afl::base::Bytes_t(buffer).trim(m_in.size())));
                    TS_ASSERT_SAME_DATA(m_in.data(), buffer, static_cast<unsigned>(m_in.size()));

                    // Send agreed response
                    TS_ASSERT_THROWS_NOTHING(io.fullWrite(afl::string::toBytes(m_out)));
                }
            }

        void setData(String_t in, String_t out)
            {
                m_in = in;
                m_out = out;
                m_wake.post();
            }

     private:
        Ref<afl::net::Listener> m_listener;
        String_t m_in;
        String_t m_out;
        afl::sys::Semaphore m_wake;
    };

    // Set up network
    uint16_t portNr = static_cast<uint16_t>(20000 + std::rand() % 10000);
    afl::net::Name name("127.0.0.1", portNr);
    afl::net::NetworkStack& stack(afl::net::NetworkStack::getInstance());

    // Make a tester
    Tester testRunnable(stack.listen(name, 5));
    afl::sys::Thread testThread("RespClientTest", testRunnable);
    testThread.start();

    // Make a client
    afl::net::resp::Client client(stack, name);

    // Test that we can do several roundtrips
    std::auto_ptr<afl::data::Value> result;
    for (int i = 0; i < 10; ++i) {
        testRunnable.setData("*2\r\n$5\r\nHello\r\n$5\r\nWorld\r\n", ":17\r\n");
        result.reset(client.call(afl::data::Segment().pushBackString("Hello").pushBackString("World")));
        TS_ASSERT(result.get() != 0);
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(result.get()) != 0);
        TS_ASSERT_EQUALS(dynamic_cast<afl::data::IntegerValue*>(result.get())->getValue(), 17);
    }

    // Test that integers are transmitted as strings
    testRunnable.setData("*2\r\n$3\r\nADD\r\n$2\r\n42\r\n", "$-1\r\n");
    result.reset(client.call(afl::data::Segment().pushBackString("ADD").pushBackInteger(42)));
    TS_ASSERT(result.get() == 0);

    // Test error
    testRunnable.setData("*0\r\n", "-ERR\r\n");
    afl::data::Segment empty;
    TS_ASSERT_THROWS(client.call(empty), afl::except::RemoteErrorException);

    // Trigger shutdown
    testRunnable.setData("", "");
}
