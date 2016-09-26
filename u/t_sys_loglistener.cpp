/**
  *  \file u/t_sys_loglistener.cpp
  *  \brief Test for afl::sys::LogListener
  */

#include "afl/sys/loglistener.hpp"

#include <stdexcept>
#include "u/t_sys.hpp"
#include "afl/except/fileproblemexception.hpp"

/** Simple test. */
void
TestSysLogListener::testIt()
{
    class Tester : public afl::sys::LogListener {
     public:
        virtual void handleMessage(const Message& msg)
            {
                m_accum += msg.m_channel;
                m_accum += "|";
                m_accum += msg.m_message;
                m_accum += "\n";
            }

        String_t m_accum;
    };
    Tester t;
    TS_ASSERT_EQUALS(t.m_accum, "");

    // write(Level, String_t, String_t)
    t.write(t.Error, "ch", "msg");
    t.write(t.Error, "ch2", "msg2");
    TS_ASSERT_EQUALS(t.m_accum, "ch|msg\nch2|msg2\n");
    t.m_accum.clear();

    // write(Level, String_t, String_t, intNr, String_t)
    t.write(t.Error, "ch", "fn", 42, "msg");
    TS_ASSERT_EQUALS(t.m_accum, "ch|fn:42: msg\n");
    t.m_accum.clear();

    t.write(t.Error, "ch", "fn", 0, "msg");
    TS_ASSERT_EQUALS(t.m_accum, "ch|fn: msg\n");
    t.m_accum.clear();

    // write(Level, String_t, String_t, std::exception)
    t.write(t.Error, "ch", "op", std::runtime_error("boom"));
    TS_ASSERT_EQUALS(t.m_accum, "ch|op: boom\n");
    t.m_accum.clear();

    t.write(t.Error, "ch", "op", afl::except::FileProblemException("fn", "msg"));
    TS_ASSERT_EQUALS(t.m_accum, "ch|fn: op: msg\n");
    t.m_accum.clear();

    t.write(t.Error, "ch", "", std::runtime_error("boom"));
    TS_ASSERT_EQUALS(t.m_accum, "ch|boom\n");
    t.m_accum.clear();

    t.write(t.Error, "ch", "", afl::except::FileProblemException("fn", "msg"));
    TS_ASSERT_EQUALS(t.m_accum, "ch|fn: msg\n");
    t.m_accum.clear();
}
