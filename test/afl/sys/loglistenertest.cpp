/**
  *  \file test/afl/sys/loglistenertest.cpp
  *  \brief Test for afl::sys::LogListener
  */

#include "afl/sys/loglistener.hpp"

#include <stdexcept>
#include "afl/test/testrunner.hpp"
#include "afl/except/fileproblemexception.hpp"

/** Simple test. */
AFL_TEST("afl.sys.LogListener", a)
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
    a.checkEqual("01. empty", t.m_accum, "");

    // write(Level, String_t, String_t)
    t.write(t.Error, "ch", "msg");
    t.write(t.Error, "ch2", "msg2");
    a.checkEqual("02. write/2", t.m_accum, "ch|msg\nch2|msg2\n");
    t.m_accum.clear();

    // write(Level, String_t, String_t, intNr, String_t)
    t.write(t.Error, "ch", "fn", 42, "msg");
    a.checkEqual("03. write/4", t.m_accum, "ch|fn:42: msg\n");
    t.m_accum.clear();

    t.write(t.Error, "ch", "fn", 0, "msg");
    a.checkEqual("04. write/4", t.m_accum, "ch|fn: msg\n");
    t.m_accum.clear();

    // write(Level, String_t, String_t, std::exception)
    t.write(t.Error, "ch", "op", std::runtime_error("boom"));
    a.checkEqual("05. write exception", t.m_accum, "ch|op: boom\n");
    t.m_accum.clear();

    t.write(t.Error, "ch", "op", afl::except::FileProblemException("fn", "msg"));
    a.checkEqual("06. write FileProblemException", t.m_accum, "ch|fn: op: msg\n");
    t.m_accum.clear();

    t.write(t.Error, "ch", "", std::runtime_error("boom"));
    a.checkEqual("07. write exception", t.m_accum, "ch|boom\n");
    t.m_accum.clear();

    t.write(t.Error, "ch", "", afl::except::FileProblemException("fn", "msg"));
    a.checkEqual("08. write FileProblemException", t.m_accum, "ch|fn: msg\n");
    t.m_accum.clear();
}
