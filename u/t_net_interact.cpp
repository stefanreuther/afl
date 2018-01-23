/**
  *  \file u/t_net_interact.cpp
  *  \brief Test for afl::net::interact
  */

#include "afl/net/interact.hpp"

#include "t_net.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/async/receiveoperation.hpp"
#include "afl/net/protocolhandler.hpp"

namespace {
    class TestComm : public afl::async::CommunicationObject {
     public:
        /*
         *  Synchronous transfer functions
         */
        virtual bool send(afl::async::Controller& /*ctl*/, afl::async::SendOperation& op, afl::sys::Timeout_t /*timeout*/)
            {
                // Add to buffer
                m_sentData += afl::string::fromBytes(op.getUnsentBytes());

                // Mark everything sent
                op.addSentBytes(op.getUnsentBytes().size());
                return true;
            }

        virtual bool receive(afl::async::Controller& /*ctl*/, afl::async::ReceiveOperation& op, afl::sys::Timeout_t /*timeout*/)
            {
                // Copy data to receive
                size_t n = op.getUnreceivedBytes().copyFrom(afl::string::toBytes(m_receiveData)).size();

                // Advance ReceiveOperation
                op.addReceivedBytes(n);

                // Remove from buffer
                m_receiveData.erase(0, n);
                return true;
            }

        /*
         *  The point of interact() is to operate synchronously, so we fail async operations.
         */
        virtual void sendAsync(afl::async::Controller& /*ctl*/, afl::async::SendOperation& /*op*/)
            { TS_ASSERT(0); }
        virtual void receiveAsync(afl::async::Controller& /*ctl*/, afl::async::ReceiveOperation& /*op*/)
            { TS_ASSERT(0); }
        virtual void cancel(afl::async::Controller& /*ctl*/, afl::async::Operation& /*op*/)
            { TS_ASSERT(0); }
        virtual String_t getName()
            { return "TestComm"; }

        /*
         *  Accessors
         */

        void addReceiveData(String_t s)
            { m_receiveData += s; }
        bool isReceiveComplete()
            { return m_receiveData.empty(); }
        String_t extractSentData()
            {
                String_t result = m_sentData;
                m_sentData.clear();
                return result;
            }

     private:
        String_t m_sentData;
        String_t m_receiveData;
    };

    enum ActionType {
        DoClose,
        DoReceive,
        DoSend,
        HandleData,
        HandleConnectionClose,
        VerifySend,
        PostReceive
    };
    struct Action {
        ActionType type;
        const char* data;
    };

    class TestPH : public afl::net::ProtocolHandler {
     public:
        TestPH(TestComm& comm, afl::base::Memory<const Action> actions)
            : m_comm(comm),
              m_actions(actions)
            { }

        virtual void getOperation(Operation& op)
            {
                update();
                const Action* p = m_actions.eat();
                TS_ASSERT(p != 0);
                if (p->type == DoClose) {
                    op.m_dataToSend = afl::base::Nothing;
                    op.m_close = true;
                    op.m_timeToWait = 0;
                } else if (p->type == DoReceive) {
                    op.m_dataToSend = afl::base::Nothing;
                    op.m_close = false;
                    op.m_timeToWait = 10000;
                } else if (p->type == DoSend) {
                    op.m_dataToSend = afl::string::toBytes(p->data);
                    op.m_close = false;
                    op.m_timeToWait = 10000;
                } else {
                    // This assertion always fails, but if it hits, cxxtest will show the failing value
                    TS_ASSERT_EQUALS(p->type, DoClose);
                }
            }
        virtual void advanceTime(afl::sys::Timeout_t /*msecs*/)
            { }
        virtual void handleData(afl::base::ConstBytes_t bytes)
            {
                const Action* p = m_actions.eat();
                TS_ASSERT(p != 0);
                TS_ASSERT_EQUALS(p->type, HandleData);
                TS_ASSERT(afl::string::toBytes(p->data).equalContent(bytes));
                update();
            }
        virtual void handleSendTimeout(afl::base::ConstBytes_t /*unsentBytes*/)
            { }
        virtual void handleConnectionClose()
            {
                const Action* p = m_actions.eat();
                TS_ASSERT(p != 0);
                TS_ASSERT_EQUALS(p->type, HandleConnectionClose);
                update();
            }
        void update()
            {
                while (const Action* p = m_actions.at(0)) {
                    if (p->type == VerifySend) {
                        TS_ASSERT_EQUALS(p->data, m_comm.extractSentData());
                        m_actions.eat();
                    } else if (p->type == PostReceive) {
                        m_comm.addReceiveData(p->data);
                        m_actions.eat();
                    } else {
                        break;
                    }
                }
            }
     private:
        TestComm& m_comm;
        afl::base::Memory<const Action> m_actions;
    };
}

/** Simple test. */
void
TestNetInteract::testIt()
{
    static const Action table[] = {
        // Tell ProtocolHandler to send some data; verify that it arrives.
        { DoSend, "hello" },
        { VerifySend, "hello" },

        // Give some data to receive, tell ProtocolHandler to receive, verify.
        { PostReceive, "welcome" },
        { DoReceive, 0 },
        { HandleData, "welcome" },

        // Request closing
        { DoClose, 0 },
        { HandleConnectionClose, 0 },
    };

    TestComm comm;
    TestPH ph(comm, table);

    interact(comm, ph);
}

