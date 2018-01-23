/**
  *  \file u/t_net_redis_stringkey.cpp
  *  \brief Test for afl::net::redis::StringKey
  */

#include "afl/net/redis/stringkey.hpp"

#include "t_net_redis.hpp"
#include "afl/data/integervalue.hpp"
#include "afl/data/stringvalue.hpp"
#include "afl/net/redis/internaldatabase.hpp"
#include "afl/test/commandhandler.hpp"

/** Test StringKey against the mock. */
void
TestNetRedisStringKey::testMock()
{
    afl::test::CommandHandler mock("testMock");
    afl::net::redis::StringKey testee(mock, "s");
    String_t s;

    // get/set
    mock.expectCall("SET, s, hello");
    mock.provideNewResult(new afl::data::IntegerValue(1));
    mock.expectCall("GET, s");
    mock.provideNewResult(new afl::data::StringValue("hello"));
    testee.set("hello");
    s = testee.get();
    TS_ASSERT_EQUALS(s, "hello");

    // string operations
    mock.expectCall("APPEND, s, world");
    mock.provideNewResult(0);
    testee.append("world");

    mock.expectCall("GETRANGE, s, 5, 6");     // fifth to sixth character, inclusive
    mock.provideNewResult(new afl::data::StringValue("wo"));
    s = testee.substr(5, 2);
    TS_ASSERT_EQUALS(s, "wo");

    mock.expectCall("GETRANGE, s, 5, 5");     // fifth character (just one)
    mock.provideNewResult(new afl::data::StringValue("w"));
    s = testee.substr(5, 1);
    TS_ASSERT_EQUALS(s, "w");

    s = testee.substr(5, 0);      // no network communication
    TS_ASSERT_EQUALS(s, "");

    mock.expectCall("STRLEN, s");
    mock.provideNewResult(new afl::data::IntegerValue(10));
    size_t n = testee.size();
    TS_ASSERT_EQUALS(n, 10U);

    // Getset
    mock.expectCall("GETSET, s, bye");
    mock.provideNewResult(new afl::data::StringValue("helloworld"));
    s = testee.replaceBy("bye");
    TS_ASSERT_EQUALS(s, "helloworld");

    // setnx
    mock.expectCall("SETNX, s, what");
    mock.provideNewResult(new afl::data::IntegerValue(0));
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
    afl::test::CommandHandler mock("testSize");
    afl::net::redis::StringKey testee(mock, "x");

    // No network communication for this one
    testee.substr(512*1024*1024 + 1, 1);

    // Truncated size
    mock.expectCall("GETRANGE, x, 1, 536870912");
    mock.provideNewResult(new afl::data::StringValue("x"));
    testee.substr(1, 512L*1024*1024+1);

    mock.expectCall("GETRANGE, x, 1, 536870912");
    mock.provideNewResult(new afl::data::StringValue("x"));
    testee.substr(1, 1024L*1024*1024);

    // Server returning bogus size
    mock.expectCall("STRLEN, x");
    mock.provideNewResult(new afl::data::StringValue("-3"));
    TS_ASSERT_EQUALS(testee.size(), 0U);

    mock.expectCall("STRLEN, x");
    mock.provideNewResult(new afl::data::IntegerValue(-3));
    TS_ASSERT_EQUALS(testee.size(), 0U);
}
