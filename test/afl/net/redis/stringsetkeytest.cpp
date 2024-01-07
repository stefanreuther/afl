/**
  *  \file test/afl/net/redis/stringsetkeytest.cpp
  *  \brief Test for afl::net::redis::StringSetKey
  */

#include "afl/net/redis/stringsetkey.hpp"

#include <algorithm>
#include "afl/data/integervalue.hpp"
#include "afl/data/stringvalue.hpp"
#include "afl/data/vector.hpp"
#include "afl/data/vectorvalue.hpp"
#include "afl/net/redis/internaldatabase.hpp"
#include "afl/net/redis/sortoperation.hpp"
#include "afl/net/redis/stringsetoperation.hpp"
#include "afl/test/commandhandler.hpp"
#include "afl/test/testrunner.hpp"

/** Test against the mock. */
AFL_TEST("afl.net.redis.StringSetKey:mock", a)
{
    afl::test::CommandHandler mock("testMock");
    afl::net::redis::StringSetKey testee(mock, "ss");

    // SetKey: size, empty
    mock.expectCall("SCARD, ss");
    mock.provideNewResult(new afl::data::IntegerValue(3));
    int32_t n = testee.size();
    a.checkEqual("01. size", n, 3);

    mock.expectCall("SCARD, ss");
    mock.provideNewResult(new afl::data::IntegerValue(3));
    bool ok = testee.empty();
    a.check("02. empty", !ok);

    // add
    mock.expectCall("SADD, ss, foo");
    mock.provideNewResult(new afl::data::IntegerValue(1));
    ok = testee.add("foo");
    a.check("11. add", ok);

    // remove
    mock.expectCall("SREM, ss, bar");
    mock.provideNewResult(new afl::data::IntegerValue(0));
    ok = testee.remove("bar");
    a.check("12. remove", !ok);

    // getAll
    mock.expectCall("SMEMBERS, ss");

    afl::base::Ref<afl::data::Vector> vec(afl::data::Vector::create());
    vec->pushBackNew(new afl::data::StringValue("foo"));
    vec->pushBackNew(new afl::data::StringValue("baz"));
    vec->pushBackNew(new afl::data::StringValue("qux"));
    mock.provideNewResult(new afl::data::VectorValue(vec));

    afl::data::StringList_t list;
    testee.getAll(list);
    a.checkEqual("21. getAll size", list.size(), 3U);
    a.checkEqual("22. getAll result", list[0], "foo");
    a.checkEqual("23. getAll result", list[1], "baz");
    a.checkEqual("24. getAll result", list[2], "qux");

    // contains
    mock.expectCall("SISMEMBER, ss, baz");
    mock.provideNewResult(new afl::data::IntegerValue(1));
    ok = testee.contains("baz");
    a.check("31. contains", ok);

    // moveTo
    mock.expectCall("SMOVE, ss, os, baz");
    mock.provideNewResult(new afl::data::IntegerValue(1));
    ok = testee.moveTo("baz", afl::net::redis::StringSetKey(mock, "os"));
    a.check("32. moveTo", ok);

    /*
     *  The following are only tested against the mock, so we don't
     *  attempt consistency between the commands and the virtual state.
     */

    // getRandom
    mock.expectCall("SRANDMEMBER, ss");
    mock.provideNewResult(new afl::data::StringValue("qux"));
    String_t s = testee.getRandom();
    a.checkEqual("41. getRandom", s, "qux");

    // extractRandom
    mock.expectCall("SPOP, ss");
    mock.provideNewResult(new afl::data::StringValue("qux"));
    s = testee.extractRandom();
    a.checkEqual("41. extractRandom", s, "qux");

    // sort
    mock.expectCall("SORT, ss");

    vec.reset(*afl::data::Vector::create());
    vec->pushBackNew(new afl::data::StringValue("alpha"));
    vec->pushBackNew(new afl::data::StringValue("bravo"));
    vec->pushBackNew(new afl::data::StringValue("charlie"));
    vec->pushBackNew(new afl::data::StringValue("delta"));
    mock.provideNewResult(new afl::data::VectorValue(vec));

    list.clear();
    testee.sort().getResult(list);
    a.checkEqual("51. sort size", list.size(), 4U);
    a.checkEqual("52. sort result", list[0], "alpha");
    a.checkEqual("53. sort result", list[1], "bravo");
    a.checkEqual("54. sort result", list[2], "charlie");
    a.checkEqual("55. sort result", list[3], "delta");

    // remove
    mock.expectCall("DEL, ss");
    mock.provideNewResult(new afl::data::IntegerValue(1));
    testee.remove();

    // copyTo
    mock.expectCall("SINTERSTORE, ts, ss");
    mock.provideNewResult(0);
    testee.copyTo(afl::net::redis::StringSetKey(mock, "ts"));

    // intersect
    mock.expectCall("SINTER, ss, ts, us");
    vec.reset(*afl::data::Vector::create());
    vec->pushBackNew(new afl::data::StringValue("res"));
    mock.provideNewResult(new afl::data::VectorValue(vec));

    list.clear();
    testee.intersect(afl::net::redis::StringSetKey(mock, "ts"))
        .andAlso(afl::net::redis::StringSetKey(mock, "us"))
        .getAll(list);
    a.checkEqual("61. intersect size", list.size(), 1U);
    a.checkEqual("62. intersect result", list[0], "res");

    // union
    mock.expectCall("SUNION, ss, ts, us");
    mock.provideNewResult(new afl::data::VectorValue(vec));

    list.clear();
    testee.merge(afl::net::redis::StringSetKey(mock, "ts"))
        .andAlso(afl::net::redis::StringSetKey(mock, "us"))
        .getAll(list);
    a.checkEqual("71. merge size", list.size(), 1U);
    a.checkEqual("72. merge result", list[0], "res");

    // difference
    mock.expectCall("SDIFF, ss, ts, us");
    mock.provideNewResult(new afl::data::VectorValue(vec));

    list.clear();
    testee.remove(afl::net::redis::StringSetKey(mock, "ts"))
        .andAlso(afl::net::redis::StringSetKey(mock, "us"))
        .getAll(list);
    a.checkEqual("81. remove size", list.size(), 1U);
    a.checkEqual("82. remove result", list[0], "res");
}

/** Test against InternalDatabase. */
AFL_TEST("afl.net.redis.StringSetKey:internal", a)
{
    afl::net::redis::InternalDatabase db;
    afl::net::redis::StringSetKey testee(db, "ss");

    // Initial content
    db.callVoid(afl::data::Segment().pushBackString("SADD").pushBackString("ss").pushBackString("baz").pushBackString("qux"));

    // SetKey: size, empty
    int32_t n = testee.size();
    a.checkEqual("01. size", n, 2);

    bool ok = testee.empty();
    a.check("02. empty", !ok);

    // add
    ok = testee.add("foo");
    a.check("11. add", ok);

    // remove
    ok = testee.remove("bar");
    a.check("12. remove", !ok);

    // getAll
    afl::data::StringList_t list;
    testee.getAll(list);
    a.checkEqual("21. getAll size", list.size(), 3U);

    std::sort(list.begin(), list.end()); // getAll does not guarantee an order!
    a.checkEqual("22. getAll result", list[0], "baz");
    a.checkEqual("23. getAll result", list[1], "foo");
    a.checkEqual("24. getAll result", list[2], "qux");

    // contains
    ok = testee.contains("baz");
    a.check("31. contains", ok);

    // moveTo
    ok = testee.moveTo("baz", afl::net::redis::StringSetKey(db, "os"));
    a.check("32. moveTo", ok);
}
