/**
  *  \file test/afl/net/resp/protocolhandlertest.cpp
  *  \brief Test for afl::net::resp::ProtocolHandler
  */

#include "afl/net/resp/protocolhandler.hpp"

#include "afl/data/defaultvaluefactory.hpp"
#include "afl/string/string.hpp"
#include "afl/sys/types.hpp"
#include "afl/test/testrunner.hpp"

/** Simple test. */
AFL_TEST("afl.net.resp.ProtocolHandler", a)
{
    // A CommandHandler that just counts the length of the array it's given
    class Tester : public afl::net::CommandHandler {
     public:
        virtual Value_t* call(const Segment_t& command)
            { return afl::data::DefaultValueFactory().createInteger(int32_t(command.size())); }
        virtual void callVoid(const Segment_t& command)
            { delete call(command); }
    };
    Tester t;

    // Instantiate testee
    afl::net::resp::ProtocolHandler testee(t);

    // Initial state must be that it wants to receive
    afl::net::ProtocolHandler::Operation op;
    op.m_dataToSend.reset();
    op.m_close = false;
    op.m_timeToWait = afl::sys::INFINITE_TIMEOUT;
    testee.getOperation(op);
    a.check("01. close", !op.m_close);
    a.checkEqual("02. dataToSend", op.m_dataToSend.size(), 0U);

    // Give them some [incomplete] data
    testee.handleData(afl::string::toBytes("one two "));
    testee.getOperation(op);
    a.check("11. close", !op.m_close);
    a.checkEqual("12. dataToSend", op.m_dataToSend.size(), 0U);

    // Give them some more data
    testee.handleData(afl::string::toBytes("three\n"));
    testee.getOperation(op);
    a.check("21. close", !op.m_close);
    a.checkEqualContent("22. dataToSend", op.m_dataToSend, afl::string::toBytes("$1\r\n3\r\n"));

    // Must again be ready for input
    testee.getOperation(op);
    a.check("31. close", !op.m_close);
    a.checkEqual("32. dataToSend", op.m_dataToSend.size(), 0U);

    // Give a bad command
    testee.handleData(afl::string::toBytes(":1\n"));
    testee.getOperation(op);
    a.check("41. close", !op.m_close);
    a.checkDifferent("42. dataToSend", op.m_dataToSend.size(), 0U);
    a.checkEqual("43. dataToSend", *op.m_dataToSend.at(0), '-');
}
