/**
  *  \file test/afl/net/redis/integersetkeytest.cpp
  *  \brief Test for afl::net::redis::IntegerSetKey
  */

#include "afl/net/redis/integersetkey.hpp"

#include <algorithm>
#include "afl/base/ptr.hpp"
#include "afl/data/integervalue.hpp"
#include "afl/data/vector.hpp"
#include "afl/data/vectorvalue.hpp"
#include "afl/net/redis/integersetoperation.hpp"
#include "afl/net/redis/internaldatabase.hpp"
#include "afl/net/redis/sortoperation.hpp"
#include "afl/test/commandhandler.hpp"
#include "afl/test/testrunner.hpp"

/** Test against the mock. */
AFL_TEST("afl.net.redis.IntegerSetKey:mock", a)
{
    afl::test::CommandHandler mock("testMock");
    afl::net::redis::IntegerSetKey testee(mock, "is");

    // SetKey: size, empty
    mock.expectCall("SCARD, is");
    mock.provideNewResult(new afl::data::IntegerValue(3));
    int32_t n = testee.size();
    a.checkEqual("size", n, 3);

    mock.expectCall("SCARD, is");
    mock.provideNewResult(new afl::data::IntegerValue(3));
    bool ok = testee.empty();
    a.check("empty", !ok);

    // add
    mock.expectCall("SADD, is, 5");
    mock.provideNewResult(new afl::data::IntegerValue(1));
    ok = testee.add(5);
    a.check("add", ok);

    // remove
    mock.expectCall("SREM, is, 3");
    mock.provideNewResult(new afl::data::IntegerValue(0));
    ok = testee.remove(3);
    a.check("remove", !ok);

    // getAll
    mock.expectCall("SMEMBERS, is");

    afl::base::Ref<afl::data::Vector> vec(afl::data::Vector::create());
    vec->pushBackNew(new afl::data::IntegerValue(1));
    vec->pushBackNew(new afl::data::IntegerValue(2));
    vec->pushBackNew(new afl::data::IntegerValue(5));
    mock.provideNewResult(new afl::data::VectorValue(vec));

    afl::data::IntegerList_t list;
    testee.getAll(list);
    a.checkEqual("getAll size", list.size(), 3U);

    std::sort(list.begin(), list.end()); // getAll does not guarantee an order!
    a.checkEqual("getAll index 0", list[0], 1);
    a.checkEqual("getAll index 1", list[1], 2);
    a.checkEqual("getAll index 2", list[2], 5);

    // contains
    mock.expectCall("SISMEMBER, is, 2");
    mock.provideNewResult(new afl::data::IntegerValue(1));
    ok = testee.contains(2);
    a.check("contains", ok);

    // moveTo
    mock.expectCall("SMOVE, is, os, 2");
    mock.provideNewResult(new afl::data::IntegerValue(1));
    ok = testee.moveTo(2, afl::net::redis::IntegerSetKey(mock, "os"));
    a.check("moveTo", ok);

    /*
     *  The following are only tested against the mock, so we don't
     *  attempt consistency between the commands and the virtual state.
     */

    // getRandom
    mock.expectCall("SRANDMEMBER, is");
    mock.provideNewResult(new afl::data::IntegerValue(2));
    n = testee.getRandom();
    a.checkEqual("getRandom", n, 2);

    // extractRandom
    mock.expectCall("SPOP, is");
    mock.provideNewResult(new afl::data::IntegerValue(5));
    n = testee.extractRandom();
    a.checkEqual("extractRandom", n, 5);

    // sort
    mock.expectCall("SORT, is");

    vec.reset(*afl::data::Vector::create());
    vec->pushBackNew(new afl::data::IntegerValue(11));
    vec->pushBackNew(new afl::data::IntegerValue(23));
    vec->pushBackNew(new afl::data::IntegerValue(42));
    vec->pushBackNew(new afl::data::IntegerValue(55));
    mock.provideNewResult(new afl::data::VectorValue(vec));

    list.clear();
    testee.sort().getResult(list);
    a.checkEqual("sort size", list.size(), 4U);
    a.checkEqual("sort index 0", list[0], 11);
    a.checkEqual("sort index 1", list[1], 23);
    a.checkEqual("sort index 2", list[2], 42);
    a.checkEqual("sort index 3", list[3], 55);

    // remove
    mock.expectCall("DEL, is");
    mock.provideNewResult(new afl::data::IntegerValue(1));
    testee.remove();

    // copyTo
    mock.expectCall("SINTERSTORE, as, is");
    mock.provideNewResult(0);
    testee.copyTo(afl::net::redis::IntegerSetKey(mock, "as"));

    // intersect
    mock.expectCall("SINTER, is, js, ks");
    vec.reset(*afl::data::Vector::create());
    vec->pushBackNew(new afl::data::IntegerValue(23));
    mock.provideNewResult(new afl::data::VectorValue(vec));

    list.clear();
    testee.intersect(afl::net::redis::IntegerSetKey(mock, "js"))
        .andAlso(afl::net::redis::IntegerSetKey(mock, "ks"))
        .getAll(list);
    a.checkEqual("intersect/getAll size", list.size(), 1U);
    a.checkEqual("intersect/getAll index 0", list[0], 23);

    // union
    mock.expectCall("SUNION, is, js, ks");
    mock.provideNewResult(new afl::data::VectorValue(vec));

    list.clear();
    testee.merge(afl::net::redis::IntegerSetKey(mock, "js"))
        .andAlso(afl::net::redis::IntegerSetKey(mock, "ks"))
        .getAll(list);
    a.checkEqual("merge/getAll size", list.size(), 1U);
    a.checkEqual("merge/getAll index 0", list[0], 23);

    // difference
    mock.expectCall("SDIFF, is, js, ks");
    mock.provideNewResult(new afl::data::VectorValue(vec));

    list.clear();
    testee.remove(afl::net::redis::IntegerSetKey(mock, "js"))
        .andAlso(afl::net::redis::IntegerSetKey(mock, "ks"))
        .getAll(list);
    a.checkEqual("remove/getAll size", list.size(), 1U);
    a.checkEqual("remove/getAll index 0", list[0], 23);
}

/** Test against InternalDatabase. */
AFL_TEST("afl.net.redis.IntegerSetKey:internal", a)
{
    afl::net::redis::InternalDatabase db;
    afl::net::redis::IntegerSetKey testee(db, "is");

    // Initial content
    db.callVoid(afl::data::Segment().pushBackString("SADD").pushBackString("is").pushBackInteger(1).pushBackInteger(2).pushBackInteger(4));

    // SetKey: size, empty
    int32_t n = testee.size();
    a.checkEqual("size", n, 3);

    bool ok = testee.empty();
    a.check("empty", !ok);

    // add
    ok = testee.add(5);
    a.check("add", ok);

    // remove
    ok = testee.remove(3);
    a.check("remove", !ok);

    // getAll
    afl::data::IntegerList_t list;
    testee.getAll(list);
    a.checkEqual("getAll size", list.size(), 4U);

    std::sort(list.begin(), list.end()); // getAll does not guarantee an order!
    a.checkEqual("getAll index 0", list[0], 1);
    a.checkEqual("getAll index 1", list[1], 2);
    a.checkEqual("getAll index 2", list[2], 4);
    a.checkEqual("getAll index 3", list[3], 5);

    // contains
    ok = testee.contains(2);
    a.check("contains", ok);

    // moveTo
    ok = testee.moveTo(2, afl::net::redis::IntegerSetKey(db, "os"));
    a.check("moveTo", ok);
    ok = testee.contains(2);
    a.check("contains 2", !ok);
}
