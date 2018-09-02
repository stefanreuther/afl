/**
  *  \file u/t_net_redis_integerfield.cpp
  *  \brief Test for afl::net::redis::IntegerField
  */

#include "afl/net/redis/integerfield.hpp"

#include "t_net_redis.hpp"
#include "afl/net/redis/hashkey.hpp"
#include "afl/test/commandhandler.hpp"
#include "afl/data/integervalue.hpp"
#include "afl/net/redis/internaldatabase.hpp"

/** Test IntegerField operations against the mock. */
void
TestNetRedisIntegerField::testMock()
{
    afl::test::CommandHandler mock("testMock");
    afl::net::redis::HashKey k(mock, "hk");
    afl::net::redis::IntegerField testee(k, "if");

    // ++
    mock.expectCall("HINCRBY, hk, if, 1");
    mock.provideNewResult(new afl::data::IntegerValue(3));
    TS_ASSERT_EQUALS(++testee, 3);

    // +=
    mock.expectCall("HINCRBY, hk, if, 7");
    mock.provideNewResult(new afl::data::IntegerValue(10));
    TS_ASSERT_EQUALS(testee += 7, 10);

    // --
    mock.expectCall("HINCRBY, hk, if, -1");
    mock.provideNewResult(new afl::data::IntegerValue(9));
    TS_ASSERT_EQUALS(--testee, 9);

    // -=
    mock.expectCall("HINCRBY, hk, if, -7");
    mock.provideNewResult(new afl::data::IntegerValue(2));
    TS_ASSERT_EQUALS(testee -= 7, 2);

    // set
    mock.expectCall("HSET, hk, if, 12");
    mock.provideNewResult(new afl::data::IntegerValue(1));
    TS_ASSERT_EQUALS(testee.set(12), true);

    // get
    mock.expectCall("HGET, hk, if");
    mock.provideNewResult(new afl::data::IntegerValue(42));
    TS_ASSERT_EQUALS(testee.get(), 42);

    // Optional
    // getOptional
    mock.expectCall("HGET, hk, if");
    mock.provideNewResult(0);
    TS_ASSERT_EQUALS(testee.getOptional().orElse(99), 99);
    mock.expectCall("HGET, hk, if");
    mock.provideNewResult(new afl::data::IntegerValue(0));
    TS_ASSERT_EQUALS(testee.getOptional().orElse(99), 0);
    mock.expectCall("HGET, hk, if");
    mock.provideNewResult(new afl::data::IntegerValue(7));
    TS_ASSERT_EQUALS(testee.getOptional().orElse(99), 7);

    mock.expectCall("HGET, hk, if");
    mock.provideNewResult(0);
    TS_ASSERT_EQUALS(testee.get(), 0);
    mock.expectCall("HGET, hk, if");
    mock.provideNewResult(new afl::data::IntegerValue(0));
    TS_ASSERT_EQUALS(testee.get(), 0);
    mock.expectCall("HGET, hk, if");
    mock.provideNewResult(new afl::data::IntegerValue(7));
    TS_ASSERT_EQUALS(testee.get(), 7);
}

/** Test IntegerField operations against InternalDatabase. */
void
TestNetRedisIntegerField::testInternal()
{
    afl::net::redis::InternalDatabase db;
    afl::net::redis::HashKey k(db, "hk");
    afl::net::redis::IntegerField testee(k, "if");

    // Set
    TS_ASSERT_EQUALS(testee.set(2), true);
    TS_ASSERT_EQUALS(testee.get(), 2);

    // Increment/decrement
    TS_ASSERT_EQUALS(++testee, 3);
    TS_ASSERT_EQUALS(testee += 7, 10);
    TS_ASSERT_EQUALS(--testee, 9);
    TS_ASSERT_EQUALS(testee -= 7, 2);

    // set/get
    TS_ASSERT_EQUALS(testee.set(12), false);
    TS_ASSERT_EQUALS(testee.get(), 12);
}

