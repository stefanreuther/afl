/**
  *  \file u/t_net_redis_stringkey.cpp
  *  \brief Test for afl::net::redis::StringKey
  */

#include "afl/net/redis/stringkey.hpp"

#include "t_net_redis.hpp"
#include "afl/data/stringvalue.hpp"
#include "afl/net/redis/internaldatabase.hpp"
#include "u/mock/commandhandlermock.hpp"
#include "afl/data/integervalue.hpp"

/** Test StringKey against the mock. */
void
TestNetRedisStringKey::testMock()
{
    CommandHandlerMock mock;
    afl::net::redis::StringKey testee(mock, "s");
    String_t s;

    // get/set
    mock.addParameterList("SET, s, hello");
    mock.addNewResult(new afl::data::IntegerValue(1));
    mock.addParameterList("GET, s");
    mock.addNewResult(new afl::data::StringValue("hello"));
    testee.set("hello");
    s = testee.get();
    TS_ASSERT_EQUALS(s, "hello");

    // string operations
    mock.addParameterList("APPEND, s, world");
    mock.addNewResult(0);
    testee.append("world");

    mock.addParameterList("GETRANGE, s, 5, 6");     // fifth to sixth character, inclusive
    mock.addNewResult(new afl::data::StringValue("wo"));
    s = testee.substr(5, 2);
    TS_ASSERT_EQUALS(s, "wo");

    mock.addParameterList("GETRANGE, s, 5, 5");     // fifth character (just one)
    mock.addNewResult(new afl::data::StringValue("w"));
    s = testee.substr(5, 1);
    TS_ASSERT_EQUALS(s, "w");

    s = testee.substr(5, 0);      // no network communication
    TS_ASSERT_EQUALS(s, "");

    mock.addParameterList("STRLEN, s");
    mock.addNewResult(new afl::data::IntegerValue(10));
    size_t n = testee.size();
    TS_ASSERT_EQUALS(n, 10U);

    // Getset
    mock.addParameterList("GETSET, s, bye");
    mock.addNewResult(new afl::data::StringValue("helloworld"));
    s = testee.replaceBy("bye");
    TS_ASSERT_EQUALS(s, "helloworld");

    // setnx
    mock.addParameterList("SETNX, s, what");
    mock.addNewResult(new afl::data::IntegerValue(0));
    TS_ASSERT(!testee.setUnique("what"));
}

/** Test StringKey against the InternalDatabase. */
void
TestNetRedisStringKey::testInternal()
{
    afl::net::redis::InternalDatabase db;
    afl::net::redis::StringKey testee(db, "k");
    String_t s;

    // get/set
    testee.set("hello");
    s = testee.get();
    TS_ASSERT_EQUALS(s, "hello");

    // string operations
    testee.append("world");

    s = testee.substr(5, 2);
    TS_ASSERT_EQUALS(s, "wo");

    s = testee.substr(5, 1);
    TS_ASSERT_EQUALS(s, "w");

    s = testee.substr(5, 0);      // no network communication
    TS_ASSERT_EQUALS(s, "");

    size_t n = testee.size();
    TS_ASSERT_EQUALS(n, 10U);

    // Getset
    s = testee.replaceBy("bye");
    TS_ASSERT_EQUALS(s, "helloworld");

    // setnx
    TS_ASSERT(!testee.setUnique("what"));
}

/** Test out-of-range sizes. */
void
TestNetRedisStringKey::testSize()
{
    CommandHandlerMock mock;
    afl::net::redis::StringKey testee(mock, "x");

    // No network communication for this one
    testee.substr(512*1024*1024 + 1, 1);

    // Truncated size
    mock.addParameterList("GETRANGE, x, 1, 536870912");
    mock.addNewResult(new afl::data::StringValue("x"));
    testee.substr(1, 512L*1024*1024+1);

    mock.addParameterList("GETRANGE, x, 1, 536870912");
    mock.addNewResult(new afl::data::StringValue("x"));
    testee.substr(1, 1024L*1024*1024);

    // Server returning bogus size
    mock.addParameterList("STRLEN, x");
    mock.addNewResult(new afl::data::StringValue("-3"));
    TS_ASSERT_EQUALS(testee.size(), 0U);

    mock.addParameterList("STRLEN, x");
    mock.addNewResult(new afl::data::IntegerValue(-3));
    TS_ASSERT_EQUALS(testee.size(), 0U);
}
