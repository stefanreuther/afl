/**
  *  \file u/t_net_resp_protocolhandler.cpp
  *  \brief Test for afl::net::resp::ProtocolHandler
  */

#include "afl/net/resp/protocolhandler.hpp"

#include "t_net_resp.hpp"
#include "afl/sys/types.hpp"
#include "afl/string/string.hpp"

/** Simple test. */
void
TestNetRespProtocolHandler::testIt()
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
    TS_ASSERT(!op.m_close);
    TS_ASSERT_EQUALS(op.m_dataToSend.size(), 0U);

    // Give them some [incomplete] data
    testee.handleData("<input>", afl::string::toBytes("one two "));
    testee.getOperation(op);
    TS_ASSERT(!op.m_close);
    TS_ASSERT_EQUALS(op.m_dataToSend.size(), 0U);

    // Give them some more data
    testee.handleData("<input>", afl::string::toBytes("three\n"));
    testee.getOperation(op);
    TS_ASSERT(!op.m_close);
    TS_ASSERT(op.m_dataToSend.equalContent(afl::string::toBytes("$1\r\n3\r\n")));

    // Must again be ready for input
    testee.getOperation(op);
    TS_ASSERT(!op.m_close);
    TS_ASSERT_EQUALS(op.m_dataToSend.size(), 0U);

    // Give a bad command
    testee.handleData("<input>", afl::string::toBytes(":1\n"));
    testee.getOperation(op);
    TS_ASSERT(!op.m_close);
    TS_ASSERT_DIFFERS(op.m_dataToSend.size(), 0U);
    TS_ASSERT_EQUALS(*op.m_dataToSend.at(0), '-');
}
