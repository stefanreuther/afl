/**
  *  \file u/t_net_commandhandler.cpp
  *  \brief Test for afl::net::CommandHandler
  */

#include "afl/net/commandhandler.hpp"

#include "u/t_net.hpp"
#include "afl/except/invaliddataexception.hpp"
#include "afl/except/remoteerrorexception.hpp"
#include "afl/data/errorvalue.hpp"

/** Test for callInt() and callString() methods. */
void
TestNetCommandHandler::testIt()
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
    TS_ASSERT_EQUALS(t.callInt(Segment().pushBack(42)), 42);
    TS_ASSERT_EQUALS(t.callString(Segment().pushBack(42)), "42");

    TS_ASSERT_EQUALS(t.callInt(Segment().pushBack("42")), 42);
    TS_ASSERT_EQUALS(t.callString(Segment().pushBack("42")), "42");

    TS_ASSERT_THROWS(t.callInt(Segment().pushBack("xy")), afl::except::InvalidDataException);
    TS_ASSERT_EQUALS(t.callString(Segment().pushBack("xy")), "xy");

    // g++-3.4.2 does not eat this if it reads callInt(Segment()) because it attempty to copy the segment?
    Segment empty;
    TS_ASSERT_EQUALS(t.callInt(empty), 0);
    TS_ASSERT_EQUALS(t.callString(empty), "");

    TS_ASSERT_THROWS(t.callInt(Segment().pushBackNew(new afl::data::ErrorValue("src", "x"))), afl::except::RemoteErrorException);
    TS_ASSERT_THROWS(t.callString(Segment().pushBackNew(new afl::data::ErrorValue("src", "x"))), afl::except::RemoteErrorException);
}
