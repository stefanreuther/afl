/**
  *  \file test/afl/net/redis/integerkeytest.cpp
  *  \brief Test for afl::net::redis::IntegerKey
  */

#include "afl/net/redis/integerkey.hpp"

#include "afl/data/integervalue.hpp"
#include "afl/net/redis/internaldatabase.hpp"
#include "afl/test/commandhandler.hpp"
#include "afl/test/testrunner.hpp"

/** Test IntegerKey against the mock. */
AFL_TEST("afl.net.redis.IntegerKey:mock", a)
{
    afl::test::CommandHandler mock("testMock");
    afl::net::redis::IntegerKey testee(mock, "k");
    int32_t n;

    // Get/set
    mock.expectCall("SET, k, 3");
    mock.provideNewResult(0);
    mock.expectCall("GET, k");
    mock.provideNewResult(new afl::data::IntegerValue(3));
    testee.set(3);
    n = testee.get();
    a.checkEqual("get", n, 3);

    // Increment/decrement
    mock.expectCall("DECR, k");
    mock.provideNewResult(new afl::data::IntegerValue(2));
    mock.expectCall("INCR, k");
    mock.provideNewResult(new afl::data::IntegerValue(3));
    mock.expectCall("INCRBY, k, 5");
    mock.provideNewResult(new afl::data::IntegerValue(8));
    mock.expectCall("DECRBY, k, 2");
    mock.provideNewResult(new afl::data::IntegerValue(6));
    n = --testee;
    a.checkEqual("op--", n, 2);
    n = ++testee;
    a.checkEqual("op++", n, 3);
    n = (testee += 5);
    a.checkEqual("op+=", n, 8);
    n = (testee -= 2);
    a.checkEqual("op-=", n, 6);

    // Getset
    mock.expectCall("GETSET, k, 4");
    mock.provideNewResult(new afl::data::IntegerValue(6));
    n = testee.replaceBy(4);
    a.checkEqual("replaceBy", n, 6);

    // setnx
    mock.expectCall("SETNX, k, 3");
    mock.provideNewResult(new afl::data::IntegerValue(0));
    a.check("setUnique", !testee.setUnique(3));

    // getOptional
    mock.expectCall("GET, k");
    mock.provideNewResult(0);
    a.checkEqual("getOptional null", testee.getOptional().orElse(99), 99);
    mock.expectCall("GET, k");
    mock.provideNewResult(new afl::data::IntegerValue(0));
    a.checkEqual("getOptional zero", testee.getOptional().orElse(99), 0);
    mock.expectCall("GET, k");
    mock.provideNewResult(new afl::data::IntegerValue(7));
    a.checkEqual("getOptional int", testee.getOptional().orElse(99), 7);

    mock.expectCall("GET, k");
    mock.provideNewResult(0);
    a.checkEqual("get null", testee.get(), 0);
    mock.expectCall("GET, k");
    mock.provideNewResult(new afl::data::IntegerValue(0));
    a.checkEqual("get zero", testee.get(), 0);
    mock.expectCall("GET, k");
    mock.provideNewResult(new afl::data::IntegerValue(7));
    a.checkEqual("get int", testee.get(), 7);
}

/** Test IntegerKey against the InternalDatabase. */
AFL_TEST("afl.net.redis.IntegerKey:internal", a)
{
    afl::net::redis::InternalDatabase db;
    afl::net::redis::IntegerKey testee(db, "k");

    // Get/set
    int32_t n;
    testee.set(3);
    n = testee.get();
    a.checkEqual("get", n, 3);

    // Increment/decrement
    n = --testee;
    a.checkEqual("op--", n, 2);
    n = ++testee;
    a.checkEqual("op++", n, 3);
    n = (testee += 5);
    a.checkEqual("op+=", n, 8);
    n = (testee -= 2);
    a.checkEqual("op-=", n, 6);

    // Getset
    n = testee.replaceBy(4);
    a.checkEqual("replaceBy", n, 6);

    // setnx
    a.check("setUnique existing", !testee.setUnique(3));

    a.check("setUnique new", afl::net::redis::IntegerKey(db, "z").setUnique(42));
    a.checkEqual("get new", afl::net::redis::IntegerKey(db, "z").get(), 42);
}
