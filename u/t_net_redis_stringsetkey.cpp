/**
  *  \file u/t_net_redis_stringsetkey.cpp
  *  \brief Test for afl::net::redis::StringSetKey
  */

#include "afl/net/redis/stringsetkey.hpp"

#include <algorithm>
#include "t_net_redis.hpp"
#include "afl/base/ptr.hpp"
#include "afl/data/integervalue.hpp"
#include "afl/data/stringvalue.hpp"
#include "afl/data/vector.hpp"
#include "afl/data/vectorvalue.hpp"
#include "afl/net/redis/internaldatabase.hpp"
#include "afl/net/redis/sortoperation.hpp"
#include "afl/net/redis/stringsetoperation.hpp"
#include "u/mock/commandhandlermock.hpp"

/** Test against the mock. */
void
TestNetRedisStringSetKey::testMock()
{
    CommandHandlerMock mock;
    afl::net::redis::StringSetKey testee(mock, "ss");

    // SetKey: size, empty
    mock.addParameterList("SCARD, ss");
    mock.addNewResult(new afl::data::IntegerValue(3));
    int32_t n = testee.size();
    TS_ASSERT_EQUALS(n, 3);

    mock.addParameterList("SCARD, ss");
    mock.addNewResult(new afl::data::IntegerValue(3));
    bool ok = testee.empty();
    TS_ASSERT(!ok);

    // add
    mock.addParameterList("SADD, ss, foo");
    mock.addNewResult(new afl::data::IntegerValue(1));
    ok = testee.add("foo");
    TS_ASSERT(ok);

    // remove
    mock.addParameterList("SREM, ss, bar");
    mock.addNewResult(new afl::data::IntegerValue(0));
    ok = testee.remove("bar");
    TS_ASSERT(!ok);

    // getAll
    mock.addParameterList("SMEMBERS, ss");

    afl::base::Ref<afl::data::Vector> vec(afl::data::Vector::create());
    vec->pushBackNew(new afl::data::StringValue("foo"));
    vec->pushBackNew(new afl::data::StringValue("baz"));
    vec->pushBackNew(new afl::data::StringValue("qux"));
    mock.addNewResult(new afl::data::VectorValue(vec));

    afl::data::StringList_t list;
    testee.getAll(list);
    TS_ASSERT_EQUALS(list.size(), 3U);
    TS_ASSERT_EQUALS(list[0], "foo");
    TS_ASSERT_EQUALS(list[1], "baz");
    TS_ASSERT_EQUALS(list[2], "qux");

    // contains
    mock.addParameterList("SISMEMBER, ss, baz");
    mock.addNewResult(new afl::data::IntegerValue(1));
    ok = testee.contains("baz");
    TS_ASSERT(ok);

    // moveTo
    mock.addParameterList("SMOVE, ss, os, baz");
    mock.addNewResult(new afl::data::IntegerValue(1));
    ok = testee.moveTo("baz", afl::net::redis::StringSetKey(mock, "os"));
    TS_ASSERT(ok);

    /*
     *  The following are only tested against the mock, so we don't
     *  attempt consistency between the commands and the virtual state.
     */

    // getRandom
    mock.addParameterList("SRANDMEMBER, ss");
    mock.addNewResult(new afl::data::StringValue("qux"));
    String_t s = testee.getRandom();
    TS_ASSERT_EQUALS(s, "qux");

    // extractRandom
    mock.addParameterList("SPOP, ss");
    mock.addNewResult(new afl::data::StringValue("qux"));
    s = testee.extractRandom();
    TS_ASSERT_EQUALS(s, "qux");

    // sort
    mock.addParameterList("SORT, ss");

    vec.reset(*afl::data::Vector::create());
    vec->pushBackNew(new afl::data::StringValue("alpha"));
    vec->pushBackNew(new afl::data::StringValue("bravo"));
    vec->pushBackNew(new afl::data::StringValue("charlie"));
    vec->pushBackNew(new afl::data::StringValue("delta"));
    mock.addNewResult(new afl::data::VectorValue(vec));

    list.clear();
    testee.sort().getResult(list);
    TS_ASSERT_EQUALS(list.size(), 4U);
    TS_ASSERT_EQUALS(list[0], "alpha");
    TS_ASSERT_EQUALS(list[1], "bravo");
    TS_ASSERT_EQUALS(list[2], "charlie");
    TS_ASSERT_EQUALS(list[3], "delta");

    // remove
    mock.addParameterList("DEL, ss");
    mock.addNewResult(new afl::data::IntegerValue(1));
    testee.remove();

    // copyTo
    mock.addParameterList("SINTERSTORE, ts, ss");
    mock.addNewResult(0);
    testee.copyTo(afl::net::redis::StringSetKey(mock, "ts"));

    // intersect
    mock.addParameterList("SINTER, ss, ts, us");
    vec.reset(*afl::data::Vector::create());
    vec->pushBackNew(new afl::data::StringValue("res"));
    mock.addNewResult(new afl::data::VectorValue(vec));

    list.clear();
    testee.intersect(afl::net::redis::StringSetKey(mock, "ts"))
        .andAlso(afl::net::redis::StringSetKey(mock, "us"))
        .getAll(list);
    TS_ASSERT_EQUALS(list.size(), 1U);
    TS_ASSERT_EQUALS(list[0], "res");

    // union
    mock.addParameterList("SUNION, ss, ts, us");
    mock.addNewResult(new afl::data::VectorValue(vec));

    list.clear();
    testee.merge(afl::net::redis::StringSetKey(mock, "ts"))
        .andAlso(afl::net::redis::StringSetKey(mock, "us"))
        .getAll(list);
    TS_ASSERT_EQUALS(list.size(), 1U);
    TS_ASSERT_EQUALS(list[0], "res");

    // difference
    mock.addParameterList("SDIFF, ss, ts, us");
    mock.addNewResult(new afl::data::VectorValue(vec));

    list.clear();
    testee.remove(afl::net::redis::StringSetKey(mock, "ts"))
        .andAlso(afl::net::redis::StringSetKey(mock, "us"))
        .getAll(list);
    TS_ASSERT_EQUALS(list.size(), 1U);
    TS_ASSERT_EQUALS(list[0], "res");
}

/** Test against InternalDatabase. */
void
TestNetRedisStringSetKey::testInternal()
{
    afl::net::redis::InternalDatabase db;
    afl::net::redis::StringSetKey testee(db, "ss");

    // Initial content
    db.callVoid(afl::data::Segment().pushBackString("SADD").pushBackString("ss").pushBackString("baz").pushBackString("qux"));

    // SetKey: size, empty
    int32_t n = testee.size();
    TS_ASSERT_EQUALS(n, 2);

    bool ok = testee.empty();
    TS_ASSERT(!ok);

    // add
    ok = testee.add("foo");
    TS_ASSERT(ok);

    // remove
    ok = testee.remove("bar");
    TS_ASSERT(!ok);

    // getAll
    afl::data::StringList_t list;
    testee.getAll(list);
    TS_ASSERT_EQUALS(list.size(), 3U);

    std::sort(list.begin(), list.end()); // getAll does not guarantee an order!
    TS_ASSERT_EQUALS(list[0], "baz");
    TS_ASSERT_EQUALS(list[1], "foo");
    TS_ASSERT_EQUALS(list[2], "qux");

    // contains
    ok = testee.contains("baz");
    TS_ASSERT(ok);

    // moveTo
    ok = testee.moveTo("baz", afl::net::redis::StringSetKey(db, "os"));
    TS_ASSERT(ok);
}

