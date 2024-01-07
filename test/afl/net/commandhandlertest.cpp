/**
  *  \file test/afl/net/commandhandlertest.cpp
  *  \brief Test for afl::net::CommandHandler
  */

#include "afl/net/commandhandler.hpp"

#include "afl/data/errorvalue.hpp"
#include "afl/except/invaliddataexception.hpp"
#include "afl/except/remoteerrorexception.hpp"
#include "afl/test/testrunner.hpp"

/** Test for callInt() and callString() methods. */
AFL_TEST("afl.net.CommandHandler", a)
{
    // Test class
    class Tester : public afl::net::CommandHandler {
     public:
        virtual void callVoid(const Segment_t&)
            { }
        virtual Value_t* call(const Segment_t& a)
            {
                return Value_t::cloneOf(a[0]);
            }
    };
    Tester t;

    // Test it
    using afl::data::Segment;
    a.checkEqual("01. int>int", t.callInt(Segment().pushBackInteger(42)), 42);
    a.checkEqual("02. int>str", t.callString(Segment().pushBackInteger(42)), "42");

    a.checkEqual("03. str>int", t.callInt(Segment().pushBackString("42")), 42);
    a.checkEqual("04. str>str", t.callString(Segment().pushBackString("42")), "42");

    AFL_CHECK_THROWS(a("05. str>int"), t.callInt(Segment().pushBackString("xy")), afl::except::InvalidDataException);
    a.checkEqual("06. str>str", t.callString(Segment().pushBackString("xy")), "xy");

    // g++-3.4.2 does not eat this if it reads callInt(Segment()) because it attempty to copy the segment?
    Segment empty;
    a.checkEqual("07. empty>int", t.callInt(empty), 0);
    a.checkEqual("08. empty>str", t.callString(empty), "");

    AFL_CHECK_THROWS(a("09. err>int"), t.callInt(Segment().pushBackNew(new afl::data::ErrorValue("src", "x"))), afl::except::RemoteErrorException);
    AFL_CHECK_THROWS(a("10. err>str"), t.callString(Segment().pushBackNew(new afl::data::ErrorValue("src", "x"))), afl::except::RemoteErrorException);
}
