/**
  *  \file u/t_net_line_protocolhandler.cpp
  *  \brief Test for afl::net::line::ProtocolHandler
  */

#include "afl/net/line/protocolhandler.hpp"

#include "t_net_line.hpp"
#include "afl/net/line/linehandler.hpp"

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

/** Test a simple query. */
void
TestNetLineProtocolHandler::testSimpleQuery()
{
    SimpleTester t;
    afl::net::line::ProtocolHandler testee(t);

    // First operation must be sending of the opening
    {
        afl::net::ProtocolHandler::Operation op;
        testee.getOperation(op);
        TS_ASSERT_EQUALS(op.m_close, false);
        TS_ASSERT_EQUALS(afl::string::fromBytes(op.m_dataToSend), "query\r\n");
    }

    // Next operation must be null
    {
        afl::net::ProtocolHandler::Operation op;
        testee.getOperation(op);
        TS_ASSERT_EQUALS(op.m_close, false);
        TS_ASSERT_EQUALS(op.m_dataToSend.size(), 0U);
    }

    // Give it some data
    testee.handleData(afl::string::toBytes("response\r\n"));
    
    // Must now request closing
    {
        afl::net::ProtocolHandler::Operation op;
        testee.getOperation(op);
        TS_ASSERT_EQUALS(op.m_close, true);
        TS_ASSERT_EQUALS(op.m_dataToSend.size(), 0U);
    }
}

