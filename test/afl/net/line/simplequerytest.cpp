/**
  *  \file test/afl/net/line/simplequerytest.cpp
  *  \brief Test for afl::net::line::SimpleQuery
  */

#include "afl/net/line/simplequery.hpp"

#include <queue>
#include "afl/net/line/linesink.hpp"
#include "afl/test/testrunner.hpp"

/** Simple test. */
AFL_TEST("afl.net.line.SimpleQuery", a)
{
    // Mock
    class LineSinkMock : public afl::net::line::LineSink {
     public:
        LineSinkMock(afl::test::Assert a)
            : m_assert(a), m_queue()
            { }
        virtual void handleLine(const String_t& line)
            {
                m_assert.check("queue empty", !m_queue.empty());
                m_assert.checkEqual("line", line, m_queue.front());
                m_queue.pop();
            }
        void expect(const String_t& line)
            { m_queue.push(line); }
        bool empty()
            { return m_queue.empty(); }

     private:
        afl::test::Assert m_assert;
        std::queue<String_t> m_queue;
    };
    LineSinkMock m(a("LineSinkMock"));
    a.check("01. empty", m.empty());

    // Testee
    afl::net::line::SimpleQuery q("the query");
    a.checkEqual("11. result", q.getResult(), "");

    // Opening
    m.expect("the query");
    a.checkEqual("21. handleOpening", q.handleOpening(m), false);
    a.check("22. empty", m.empty());

    // Response
    a.checkEqual("31. handleLine", q.handleLine("r1", m), false);
    a.checkEqual("32. handleLine", q.handleLine("r2", m), false);
    a.checkEqual("33. handleLine", q.handleLine("r3", m), false);
    q.handleConnectionClose();

    // Result
    a.checkEqual("41. getResult", q.getResult(), "r1\nr2\nr3\n");
}
