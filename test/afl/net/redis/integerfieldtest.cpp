/**
  *  \file test/afl/net/redis/integerfieldtest.cpp
  *  \brief Test for afl::net::redis::IntegerField
  */

#include "afl/net/redis/integerfield.hpp"
#include "afl/test/testrunner.hpp"

#include "afl/net/redis/hashkey.hpp"
#include "afl/test/commandhandler.hpp"
#include "afl/data/integervalue.hpp"
#include "afl/net/redis/internaldatabase.hpp"

/** Test IntegerField operations against the mock. */
AFL_TEST("afl.net.redis.IntegerField:mock", a)
{
    afl::test::CommandHandler mock("testMock");
    afl::net::redis::HashKey k(mock, "hk");
    afl::net::redis::IntegerField testee(k, "if");

    // ++
    mock.expectCall("HINCRBY, hk, if, 1");
    mock.provideNewResult(new afl::data::IntegerValue(3));
    a.checkEqual("op++", ++testee, 3);

    // +=
    mock.expectCall("HINCRBY, hk, if, 7");
    mock.provideNewResult(new afl::data::IntegerValue(10));
    a.checkEqual("op+=", testee += 7, 10);

    // --
    mock.expectCall("HINCRBY, hk, if, -1");
    mock.provideNewResult(new afl::data::IntegerValue(9));
    a.checkEqual("op--", --testee, 9);

    // -=
    mock.expectCall("HINCRBY, hk, if, -7");
    mock.provideNewResult(new afl::data::IntegerValue(2));
    a.checkEqual("op-=", testee -= 7, 2);

    // set
    mock.expectCall("HSET, hk, if, 12");
    mock.provideNewResult(new afl::data::IntegerValue(1));
    a.checkEqual("set", testee.set(12), true);

    // get
    mock.expectCall("HGET, hk, if");
    mock.provideNewResult(new afl::data::IntegerValue(42));
    a.checkEqual("get", testee.get(), 42);

    // Optional
    // getOptional
    mock.expectCall("HGET, hk, if");
    mock.provideNewResult(0);
    a.checkEqual("getOptional null", testee.getOptional().orElse(99), 99);
    mock.expectCall("HGET, hk, if");
    mock.provideNewResult(new afl::data::IntegerValue(0));
    a.checkEqual("getOptional zero", testee.getOptional().orElse(99), 0);
    mock.expectCall("HGET, hk, if");
    mock.provideNewResult(new afl::data::IntegerValue(7));
    a.checkEqual("getOptional int", testee.getOptional().orElse(99), 7);

    mock.expectCall("HGET, hk, if");
    mock.provideNewResult(0);
    a.checkEqual("get null", testee.get(), 0);
    mock.expectCall("HGET, hk, if");
    mock.provideNewResult(new afl::data::IntegerValue(0));
    a.checkEqual("get zero", testee.get(), 0);
    mock.expectCall("HGET, hk, if");
    mock.provideNewResult(new afl::data::IntegerValue(7));
    a.checkEqual("get int", testee.get(), 7);
}

/** Test IntegerField operations against InternalDatabase. */
AFL_TEST("afl.net.redis.IntegerField:internal", a)
{
    afl::net::redis::InternalDatabase db;
    afl::net::redis::HashKey k(db, "hk");
    afl::net::redis::IntegerField testee(k, "if");

    // Set
    a.checkEqual("set new", testee.set(2), true);
    a.checkEqual("get", testee.get(), 2);

    // Increment/decrement
    a.checkEqual("op++", ++testee, 3);
    a.checkEqual("op+=", testee += 7, 10);
    a.checkEqual("op--", --testee, 9);
    a.checkEqual("op-=", testee -= 7, 2);

    // set/get
    a.checkEqual("set existing", testee.set(12), false);
    a.checkEqual("get existing", testee.get(), 12);
}
