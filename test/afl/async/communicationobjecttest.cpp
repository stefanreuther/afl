/**
  *  \file test/afl/async/communicationobjecttest.cpp
  *  \brief Test for afl::async::CommunicationObject
  */

#include "afl/async/communicationobject.hpp"

#include <stdexcept>
#include "afl/async/controller.hpp"
#include "afl/async/receiveoperation.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/test/testrunner.hpp"

using afl::except::FileProblemException;

/** Simple test. */
AFL_TEST_NOARG("afl.async.CommunicationObject:interface")
{
    /* It's an interface; we only test that the header file is valid. */
}

/** Test fullSend(). */
AFL_TEST("afl.async.CommunicationObject:fullSend", a)
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
            { throw std::runtime_error("sendAsync unexpected"); }
        virtual bool receive(afl::async::Controller& /*ctl*/, afl::async::ReceiveOperation& /*op*/, afl::sys::Timeout_t /*timeout*/)
            { throw std::runtime_error("receive unexpected"); }
        virtual void receiveAsync(afl::async::Controller& /*ctl*/, afl::async::ReceiveOperation& /*op*/)
            { throw std::runtime_error("receiveAsync unexpected"); }
        virtual void cancel(afl::async::Controller& /*ctl*/, afl::async::Operation& /*op*/)
            { throw std::runtime_error("cancel unexpected"); }
        virtual String_t getName()
            { return String_t(); }
     private:
        size_t m_bytesToSend;
    };

    static const uint8_t THREE_BYTES[] = {32,16,8};

    // Success case
    afl::async::Controller ctl;
    AFL_CHECK_SUCCEEDS(a("send 3"), SendTester(3).fullSend(ctl, THREE_BYTES));
    AFL_CHECK_SUCCEEDS(a("send 100"), SendTester(100).fullSend(ctl, THREE_BYTES));

    // Error cases
    AFL_CHECK_THROWS(a("send 0"), SendTester(0).fullSend(ctl, THREE_BYTES), FileProblemException);
    AFL_CHECK_THROWS(a("send 2"), SendTester(2).fullSend(ctl, THREE_BYTES), FileProblemException);
}

/** Test fullReceive(). */
AFL_TEST("afl.async.CommunicationObject:fullReceive", a)
{
    class ReceiveTester : public afl::async::CommunicationObject {
     public:
        ReceiveTester(size_t bytesToReceive)
            : m_bytesToReceive(bytesToReceive)
            { }
        virtual bool send(afl::async::Controller& /*ctl*/, afl::async::SendOperation& /*op*/, afl::sys::Timeout_t /*timeout*/)
            { throw std::runtime_error("send unexpected"); }
        virtual void sendAsync(afl::async::Controller& /*ctl*/, afl::async::SendOperation& /*op*/)
            { throw std::runtime_error("sendAsync unexpected"); }
        virtual bool receive(afl::async::Controller& /*ctl*/, afl::async::ReceiveOperation& op, afl::sys::Timeout_t /*timeout*/)
            {
                if (m_bytesToReceive != 0) {
                    op.addReceivedBytes(1);
                    --m_bytesToReceive;
                }
                return true;
            }
        virtual void receiveAsync(afl::async::Controller& /*ctl*/, afl::async::ReceiveOperation& /*op*/)
            { throw std::runtime_error("receiveAsync unexpected"); }
        virtual void cancel(afl::async::Controller& /*ctl*/, afl::async::Operation& /*op*/)
            { throw std::runtime_error("cancel unexpected"); }
        virtual String_t getName()
            { return String_t(); }
     private:
        size_t m_bytesToReceive;
    };

    uint8_t THREE_BYTES[3];

    // Success case
    afl::async::Controller ctl;
    AFL_CHECK_SUCCEEDS(a("receive 3"), ReceiveTester(3).fullReceive(ctl, THREE_BYTES));
    AFL_CHECK_SUCCEEDS(a("receive 100"), ReceiveTester(100).fullReceive(ctl, THREE_BYTES));

    // Error cases
    AFL_CHECK_THROWS(a("receive 0"), ReceiveTester(0).fullReceive(ctl, THREE_BYTES), FileProblemException);
    AFL_CHECK_THROWS(a("receive 2"), ReceiveTester(2).fullReceive(ctl, THREE_BYTES), FileProblemException);
}
