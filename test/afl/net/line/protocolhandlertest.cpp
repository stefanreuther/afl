/**
  *  \file test/afl/net/line/protocolhandlertest.cpp
  *  \brief Test for afl::net::line::ProtocolHandler
  */

#include "afl/net/line/protocolhandler.hpp"

#include "afl/net/line/linehandler.hpp"
#include "afl/test/testrunner.hpp"

namespace {
    class SimpleTester : public afl::net::line::LineHandler {
     public:
        SimpleTester(afl::test::Assert a)
            : m_assert(a),
              m_hadOpening(false),
              m_hadLine(false)
            { }
        virtual bool handleOpening(afl::net::line::LineSink& response)
            {
                m_assert.check("had no opening yet", !m_hadOpening);
                m_hadOpening = true;
                response.handleLine("query");
                return false;
            }

        virtual bool handleLine(const String_t& line, afl::net::line::LineSink& /*response*/)
            {
                m_assert.check("had no line", !m_hadLine);
                m_assert.checkEqual("correct response", line, "response");
                m_hadLine = true;
                return true;
            }

        virtual void handleConnectionClose()
            {
                // We don't expect this call
                m_assert.fail("unexpected: handleConnectionClose");
            }
     private:
        afl::test::Assert m_assert;
        bool m_hadOpening;
        bool m_hadLine;
    };
}

/** Test a simple query. */
AFL_TEST("afl.net.line.ProtocolHandler", a)
{
    SimpleTester t(a("SimpleTester"));
    afl::net::line::ProtocolHandler testee(t);

    // First operation must be sending of the opening
    {
        afl::net::ProtocolHandler::Operation op;
        testee.getOperation(op);
        a.checkEqual("01. close", op.m_close, false);
        a.checkEqual("02. data", afl::string::fromBytes(op.m_dataToSend), "query\r\n");
    }

    // Next operation must be null
    {
        afl::net::ProtocolHandler::Operation op;
        testee.getOperation(op);
        a.checkEqual("11. close", op.m_close, false);
        a.checkEqual("12. data", op.m_dataToSend.size(), 0U);
    }

    // Give it some data
    testee.handleData(afl::string::toBytes("response\r\n"));

    // Must now request closing
    {
        afl::net::ProtocolHandler::Operation op;
        testee.getOperation(op);
        a.checkEqual("21. close", op.m_close, true);
        a.checkEqual("22. data", op.m_dataToSend.size(), 0U);
    }
}
