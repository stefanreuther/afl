/**
  *  \file u/t_net_redis_integerkey.cpp
  *  \brief Test for afl::net::redis::IntegerKey
  */

#include "afl/net/redis/integerkey.hpp"

#include "t_net_redis.hpp"
#include "afl/data/integervalue.hpp"
#include "afl/net/redis/internaldatabase.hpp"
#include "u/mock/commandhandlermock.hpp"

/** Test IntegerKey against the mock. */
void
TestNetRedisIntegerKey::testMock()
{
    CommandHandlerMock mock;
    afl::net::redis::IntegerKey testee(mock, "k");
    int32_t n;

    // Get/set
    mock.addParameterList("SET, k, 3");
    mock.addNewResult(0);
    mock.addParameterList("GET, k");
    mock.addNewResult(new afl::data::IntegerValue(3));
    testee.set(3);
    n = testee.get();
    TS_ASSERT_EQUALS(n, 3);

    // Increment/decrement
    mock.addParameterList("DECR, k");
    mock.addNewResult(new afl::data::IntegerValue(2));
    mock.addParameterList("INCR, k");
    mock.addNewResult(new afl::data::IntegerValue(3));
    mock.addParameterList("INCRBY, k, 5");
    mock.addNewResult(new afl::data::IntegerValue(8));
    mock.addParameterList("DECRBY, k, 2");
    mock.addNewResult(new afl::data::IntegerValue(6));
    n = --testee;
    TS_ASSERT_EQUALS(n, 2);
    n = ++testee;
    TS_ASSERT_EQUALS(n, 3);
    n = (testee += 5);
    TS_ASSERT_EQUALS(n, 8);
    n = (testee -= 2);
    TS_ASSERT_EQUALS(n, 6);

    // Getset
    mock.addParameterList("GETSET, k, 4");
    mock.addNewResult(new afl::data::IntegerValue(6));
    n = testee.replaceBy(4);
    TS_ASSERT_EQUALS(n, 6);

    // setnx
    mock.addParameterList("SETNX, k, 3");
    mock.addNewResult(new afl::data::IntegerValue(0));
    TS_ASSERT(!testee.setUnique(3));
}

/** Test IntegerKey against the InternalDatabase. */
void
TestNetRedisIntegerKey::testInternal()
{
    afl::net::redis::InternalDatabase db;
    afl::net::redis::IntegerKey testee(db, "k");
    
    // Get/set
    int32_t n;
    testee.set(3);
    n = testee.get();
    TS_ASSERT_EQUALS(n, 3);

    // Increment/decrement
    n = --testee;
    TS_ASSERT_EQUALS(n, 2);
    n = ++testee;
    TS_ASSERT_EQUALS(n, 3);
    n = (testee += 5);
    TS_ASSERT_EQUALS(n, 8);
    n = (testee -= 2);
    TS_ASSERT_EQUALS(n, 6);

    // Getset
    n = testee.replaceBy(4);
    TS_ASSERT_EQUALS(n, 6);

    // setnx
    TS_ASSERT(!testee.setUnique(3));

    TS_ASSERT(afl::net::redis::IntegerKey(db, "z").setUnique(42));
    TS_ASSERT_EQUALS(afl::net::redis::IntegerKey(db, "z").get(), 42);
}

