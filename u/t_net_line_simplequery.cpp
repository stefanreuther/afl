/**
  *  \file u/t_net_line_simplequery.cpp
  *  \brief Test for afl::net::line::SimpleQuery
  */

#include <queue>
#include "afl/net/line/simplequery.hpp"

#include "t_net_line.hpp"
#include "afl/net/line/linesink.hpp"

/** Simple test. */
void
TestNetLineSimpleQuery::testIt()
{
    // Mock
    class LineSinkMock : public afl::net::line::LineSink {
     public:
        virtual void handleLine(const String_t& line)
            {
                TS_ASSERT(!m_queue.empty());
                TS_ASSERT_EQUALS(line, m_queue.front());
                m_queue.pop();
            }
        void expect(const String_t& line)
            { m_queue.push(line); }
        bool empty()
            { return m_queue.empty(); }

     private:
        std::queue<String_t> m_queue;
    };
    LineSinkMock m;
    TS_ASSERT(m.empty());

    // Testee
    afl::net::line::SimpleQuery q("the query");
    TS_ASSERT_EQUALS(q.getResult(), "");

    // Opening
    m.expect("the query");
    TS_ASSERT_EQUALS(q.handleOpening(m), false);
    TS_ASSERT(m.empty());

    // Response
    TS_ASSERT_EQUALS(q.handleLine("r1", m), false);
    TS_ASSERT_EQUALS(q.handleLine("r2", m), false);
    TS_ASSERT_EQUALS(q.handleLine("r3", m), false);
    q.handleConnectionClose();

    // Result
    TS_ASSERT_EQUALS(q.getResult(), "r1\nr2\nr3\n");
}

