/**
  *  \file u/t_async_communicationobject.cpp
  *  \brief Test for afl::async::CommunicationObject
  */

#include "afl/async/communicationobject.hpp"

#include "u/t_async.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/async/receiveoperation.hpp"
#include "afl/async/controller.hpp"
#include "afl/except/fileproblemexception.hpp"

/** Simple test. */
void
TestAsyncCommunicationObject::testIt()
{
    /* It's an interface; we only test that the header file is valid. */
}

/** Test fullSend(). */
void
TestAsyncCommunicationObject::testFullSend()
{
    /*
     *  Simple tester for sending: perform send requests piece-wise, sending one byte each.
     *  All calls other than send() are rejected.
     *
     *  Limitations:
     *  - does not test the timeout case
     *  - this limits the implementation to not use a sendAsync / cancel combo.
     */
    class SendTester : public afl::async::CommunicationObject {
     public:
        SendTester(size_t bytesToSend)
            : m_bytesToSend(bytesToSend)
            { }
        virtual bool send(afl::async::Controller& /*ctl*/, afl::async::SendOperation& op, afl::sys::Timeout_t /*timeout*/)
            {
                if (m_bytesToSend != 0) {
                    op.addSentBytes(1);
                    --m_bytesToSend;
                }
                return true;
            }
        virtual void sendAsync(afl::async::Controller& /*ctl*/, afl::async::SendOperation& /*op*/)
            { TS_ASSERT(0); }
        virtual bool receive(afl::async::Controller& /*ctl*/, afl::async::ReceiveOperation& /*op*/, afl::sys::Timeout_t /*timeout*/)
            { TS_ASSERT(0); return false; }
        virtual void receiveAsync(afl::async::Controller& /*ctl*/, afl::async::ReceiveOperation& /*op*/)
            { TS_ASSERT(0); }
        virtual void cancel(afl::async::Controller& /*ctl*/, afl::async::Operation& /*op*/)
            { TS_ASSERT(0); }
        virtual String_t getName()
            { return String_t(); }
     private:
        size_t m_bytesToSend;
    };

    static const uint8_t THREE_BYTES[] = {32,16,8};

    // Success case
    afl::async::Controller ctl;
    TS_ASSERT_THROWS_NOTHING(SendTester(3).fullSend(ctl, THREE_BYTES));
    TS_ASSERT_THROWS_NOTHING(SendTester(100).fullSend(ctl, THREE_BYTES));

    // Error cases
    TS_ASSERT_THROWS(SendTester(0).fullSend(ctl, THREE_BYTES), afl::except::FileProblemException);
    TS_ASSERT_THROWS(SendTester(2).fullSend(ctl, THREE_BYTES), afl::except::FileProblemException);
}

/** Test fullReceive(). */
void
TestAsyncCommunicationObject::testFullReceive()
{
    class ReceiveTester : public afl::async::CommunicationObject {
     public:
        ReceiveTester(size_t bytesToReceive)
            : m_bytesToReceive(bytesToReceive)
            { }
        virtual bool send(afl::async::Controller& /*ctl*/, afl::async::SendOperation& /*op*/, afl::sys::Timeout_t /*timeout*/)
            { TS_ASSERT(0); return false; }
        virtual void sendAsync(afl::async::Controller& /*ctl*/, afl::async::SendOperation& /*op*/)
            { TS_ASSERT(0); }
        virtual bool receive(afl::async::Controller& /*ctl*/, afl::async::ReceiveOperation& op, afl::sys::Timeout_t /*timeout*/)
            {
                if (m_bytesToReceive != 0) {
                    op.addReceivedBytes(1);
                    --m_bytesToReceive;
                }
                return true;
            }
        virtual void receiveAsync(afl::async::Controller& /*ctl*/, afl::async::ReceiveOperation& /*op*/)
            { TS_ASSERT(0); }
        virtual void cancel(afl::async::Controller& /*ctl*/, afl::async::Operation& /*op*/)
            { TS_ASSERT(0); }
        virtual String_t getName()
            { return String_t(); }
     private:
        size_t m_bytesToReceive;
    };

    uint8_t THREE_BYTES[3];

    // Success case
    afl::async::Controller ctl;
    TS_ASSERT_THROWS_NOTHING(ReceiveTester(3).fullReceive(ctl, THREE_BYTES));
    TS_ASSERT_THROWS_NOTHING(ReceiveTester(100).fullReceive(ctl, THREE_BYTES));

    // Error cases
    TS_ASSERT_THROWS(ReceiveTester(0).fullReceive(ctl, THREE_BYTES), afl::except::FileProblemException);
    TS_ASSERT_THROWS(ReceiveTester(2).fullReceive(ctl, THREE_BYTES), afl::except::FileProblemException);
}

