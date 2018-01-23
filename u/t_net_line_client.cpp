/**
  *  \file u/t_net_line_client.cpp
  *  \brief Test for afl::net::line::Client
  */

#include "afl/net/line/client.hpp"

#include "t_net_line.hpp"
#include "afl/async/receiveoperation.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/base/stoppable.hpp"
#include "afl/net/line/linehandler.hpp"
#include "afl/net/line/linesink.hpp"
#include "afl/sys/thread.hpp"

namespace {
    class SimpleTester : public afl::net::line::LineHandler {
     public:
        SimpleTester()
            : m_hadOpening(false),
              m_hadLine(false)
            { }
        virtual bool handleOpening(afl::net::line::LineSink& response)
            {
                TS_ASSERT(!m_hadOpening);
                m_hadOpening = true;
                response.handleLine("query");
                return false;
            }

        virtual bool handleLine(const String_t& line, afl::net::line::LineSink& /*response*/)
            {
                TS_ASSERT(!m_hadLine);
                TS_ASSERT_EQUALS(line, "response");
                m_hadLine = true;
                return true;
            }

        virtual void handleConnectionClose()
            {
                // We don't expect this call
                TS_ASSERT(false);
            }
     private:
        bool m_hadOpening;
        bool m_hadLine;
    };
}

void
TestNetLineClient::testSimpleQuery()
{
    // Set up network
    afl::net::NetworkStack& ns = afl::net::NetworkStack::getInstance();
    afl::net::Name name("127.0.0.1", uint16_t(std::rand() % 10000 + 20000));
    afl::base::Ref<afl::net::Listener> listener = ns.listen(name, 10);

    // Create a server that serves N requests
    static const int NUM_REQUESTS = 1;
    class Server : public afl::base::Stoppable {
     public:
        Server(afl::net::Listener& listener)
            : m_listener(listener),
              m_query(),
              m_answer()
            { }
        void set(String_t q, String_t a)
            { m_query = q; m_answer = a; }
        void run()
            {
                for (int i = 0; i < NUM_REQUESTS; ++i) {
                    // Accept a connection
                    afl::base::Ptr<afl::net::Socket> s = m_listener.accept();

                    // Read a request. Request ends at \r\n
                    String_t line;
                    afl::async::Controller ctl;
                    while (1) {
                        uint8_t buf[1];
                        afl::async::ReceiveOperation rx(buf);
                        s->receive(ctl, rx);
                        if (rx.getNumReceivedBytes() == 0) {
                            // Syntax error on input
                            break;
                        }
                        if (buf[0] == '\r') {
                            // ignore
                        } else if (buf[0] == '\n') {
                            // count
                            break;
                        } else {
                            line += char(buf[0]);
                        }
                    }
                    TS_ASSERT_EQUALS(line, m_query);

                    // Send a response
                    afl::async::SendOperation tx(afl::string::toBytes(m_answer + "\r\n"));
                    while (tx.getUnsentBytes().size() != 0) {
                        tx.setData(tx.getUnsentBytes());
                        s->send(ctl, tx);
                    }
                }
            }
        void stop()
            { }
     private:
        afl::net::Listener& m_listener;
        String_t m_query;
        String_t m_answer;
    };
    Server server(*listener);
    afl::sys::Thread serverThread("serverThread", server);
    serverThread.start();

    // Test
    afl::net::line::Client testee(ns, name);

    // Query
    SimpleTester t;
    server.set("query", "response");
    testee.call(t);
}

