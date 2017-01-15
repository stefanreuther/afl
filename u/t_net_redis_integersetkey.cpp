/**
  *  \file u/t_net_redis_integersetkey.cpp
  *  \brief Test for afl::net::redis::IntegerSetKey
  */

#include "afl/net/redis/integersetkey.hpp"

#include <algorithm>
#include "t_net_redis.hpp"
#include "afl/base/ptr.hpp"
#include "afl/data/integervalue.hpp"
#include "afl/data/vector.hpp"
#include "afl/data/vectorvalue.hpp"
#include "afl/net/redis/integersetoperation.hpp"
#include "afl/net/redis/internaldatabase.hpp"
#include "afl/net/redis/sortoperation.hpp"
#include "u/mock/commandhandlermock.hpp"

/** Test against the mock. */
void
TestNetRedisIntegerSetKey::testMock()
{
    CommandHandlerMock mock;
    afl::net::redis::IntegerSetKey testee(mock, "is");

    // SetKey: size, empty
    mock.addParameterList("SCARD, is");
    mock.addNewResult(new afl::data::IntegerValue(3));
    int32_t n = testee.size();
    TS_ASSERT_EQUALS(n, 3);

    mock.addParameterList("SCARD, is");
    mock.addNewResult(new afl::data::IntegerValue(3));
    bool ok = testee.empty();
    TS_ASSERT(!ok);

    // add
    mock.addParameterList("SADD, is, 5");
    mock.addNewResult(new afl::data::IntegerValue(1));
    ok = testee.add(5);
    TS_ASSERT(ok);

    // remove
    mock.addParameterList("SREM, is, 3");
    mock.addNewResult(new afl::data::IntegerValue(0));
    ok = testee.remove(3);
    TS_ASSERT(!ok);

    // getAll
    mock.addParameterList("SMEMBERS, is");

    afl::base::Ref<afl::data::Vector> vec(afl::data::Vector::create());
    vec->pushBackNew(new afl::data::IntegerValue(1));
    vec->pushBackNew(new afl::data::IntegerValue(2));
    vec->pushBackNew(new afl::data::IntegerValue(5));
    mock.addNewResult(new afl::data::VectorValue(vec));

    afl::data::IntegerList_t list;
    testee.getAll(list);
    TS_ASSERT_EQUALS(list.size(), 3U);

    std::sort(list.begin(), list.end()); // getAll does not guarantee an order!
    TS_ASSERT_EQUALS(list[0], 1);
    TS_ASSERT_EQUALS(list[1], 2);
    TS_ASSERT_EQUALS(list[2], 5);

    // contains
    mock.addParameterList("SISMEMBER, is, 2");
    mock.addNewResult(new afl::data::IntegerValue(1));
    ok = testee.contains(2);
    TS_ASSERT(ok);

    // moveTo
    mock.addParameterList("SMOVE, is, os, 2");
    mock.addNewResult(new afl::data::IntegerValue(1));
    ok = testee.moveTo(2, afl::net::redis::IntegerSetKey(mock, "os"));
    TS_ASSERT(ok);

    /*
     *  The following are only tested against the mock, so we don't
     *  attempt consistency between the commands and the virtual state.
     */

    // getRandom
    mock.addParameterList("SRANDMEMBER, is");
    mock.addNewResult(new afl::data::IntegerValue(2));
    n = testee.getRandom();
    TS_ASSERT_EQUALS(n, 2);

    // extractRandom
    mock.addParameterList("SPOP, is");
    mock.addNewResult(new afl::data::IntegerValue(5));
    n = testee.extractRandom();
    TS_ASSERT_EQUALS(n, 5);

    // sort
    mock.addParameterList("SORT, is");

    vec.reset(*afl::data::Vector::create());
    vec->pushBackNew(new afl::data::IntegerValue(11));
    vec->pushBackNew(new afl::data::IntegerValue(23));
    vec->pushBackNew(new afl::data::IntegerValue(42));
    vec->pushBackNew(new afl::data::IntegerValue(55));
    mock.addNewResult(new afl::data::VectorValue(vec));

    list.clear();
    testee.sort().getResult(list);
    TS_ASSERT_EQUALS(list.size(), 4U);
    TS_ASSERT_EQUALS(list[0], 11);
    TS_ASSERT_EQUALS(list[1], 23);
    TS_ASSERT_EQUALS(list[2], 42);
    TS_ASSERT_EQUALS(list[3], 55);

    // remove
    mock.addParameterList("DEL, is");
    mock.addNewResult(new afl::data::IntegerValue(1));
    testee.remove();

    // copyTo
    mock.addParameterList("SINTERSTORE, as, is");
    mock.addNewResult(0);
    testee.copyTo(afl::net::redis::IntegerSetKey(mock, "as"));

    // intersect
    mock.addParameterList("SINTER, is, js, ks");
    vec.reset(*afl::data::Vector::create());
    vec->pushBackNew(new afl::data::IntegerValue(23));
    mock.addNewResult(new afl::data::VectorValue(vec));

    list.clear();
    testee.intersect(afl::net::redis::IntegerSetKey(mock, "js"))
        .andAlso(afl::net::redis::IntegerSetKey(mock, "ks"))
        .getAll(list);
    TS_ASSERT_EQUALS(list.size(), 1U);
    TS_ASSERT_EQUALS(list[0], 23);

    // union
    mock.addParameterList("SUNION, is, js, ks");
    mock.addNewResult(new afl::data::VectorValue(vec));

    list.clear();
    testee.merge(afl::net::redis::IntegerSetKey(mock, "js"))
        .andAlso(afl::net::redis::IntegerSetKey(mock, "ks"))
        .getAll(list);
    TS_ASSERT_EQUALS(list.size(), 1U);
    TS_ASSERT_EQUALS(list[0], 23);

    // difference
    mock.addParameterList("SDIFF, is, js, ks");
    mock.addNewResult(new afl::data::VectorValue(vec));

    list.clear();
    testee.remove(afl::net::redis::IntegerSetKey(mock, "js"))
        .andAlso(afl::net::redis::IntegerSetKey(mock, "ks"))
        .getAll(list);
    TS_ASSERT_EQUALS(list.size(), 1U);
    TS_ASSERT_EQUALS(list[0], 23);
}

/** Test against InternalDatabase. */
void
TestNetRedisIntegerSetKey::testInternal()
{
    afl::net::redis::InternalDatabase db;
    afl::net::redis::IntegerSetKey testee(db, "is");

    // Initial content
    db.callVoid(afl::data::Segment().pushBackString("SADD").pushBackString("is").pushBackInteger(1).pushBackInteger(2).pushBackInteger(4));

    // SetKey: size, empty
    int32_t n = testee.size();
    TS_ASSERT_EQUALS(n, 3);

    bool ok = testee.empty();
    TS_ASSERT(!ok);

    // add
    ok = testee.add(5);
    TS_ASSERT(ok);

    // remove
    ok = testee.remove(3);
    TS_ASSERT(!ok);

    // getAll
    afl::data::IntegerList_t list;
    testee.getAll(list);
    TS_ASSERT_EQUALS(list.size(), 4U);

    std::sort(list.begin(), list.end()); // getAll does not guarantee an order!
    TS_ASSERT_EQUALS(list[0], 1);
    TS_ASSERT_EQUALS(list[1], 2);
    TS_ASSERT_EQUALS(list[2], 4);
    TS_ASSERT_EQUALS(list[3], 5);

    // contains
    ok = testee.contains(2);
    TS_ASSERT(ok);

    // moveTo
    ok = testee.moveTo(2, afl::net::redis::IntegerSetKey(db, "os"));
    TS_ASSERT(ok);
    ok = testee.contains(2);
    TS_ASSERT(!ok);
}
