/**
  *  \file test/afl/net/redis/keytest.cpp
  *  \brief Test for afl::net::redis::Key
  */

#include "afl/net/redis/key.hpp"

#include "afl/data/integervalue.hpp"
#include "afl/data/stringvalue.hpp"
#include "afl/net/redis/hashkey.hpp"
#include "afl/net/redis/integerfield.hpp"
#include "afl/net/redis/integerkey.hpp"
#include "afl/net/redis/integerlistkey.hpp"
#include "afl/net/redis/integersetkey.hpp"
#include "afl/net/redis/internaldatabase.hpp"
#include "afl/net/redis/stringkey.hpp"
#include "afl/net/redis/stringlistkey.hpp"
#include "afl/net/redis/stringsetkey.hpp"
#include "afl/test/commandhandler.hpp"
#include "afl/test/testrunner.hpp"

/** Test Key against the mock. */
AFL_TEST("afl.net.redis.Key:mock", a)
{
    afl::test::CommandHandler mock("testMock");
    afl::net::redis::Key testee(mock, "k");

    // Local inquiry
    a.checkEqual("getName", testee.getName(), "k");
    a.checkEqual("getHandler", &testee.getHandler(), &mock);

    // Network inquiry
    mock.expectCall("EXISTS, k");
    mock.provideNewResult(new afl::data::IntegerValue(1));
    bool ok = testee.exists();
    a.check("exists ok", ok);

    mock.expectCall("TYPE, k");
    mock.provideNewResult(new afl::data::StringValue("set"));
    a.check("getType result", testee.getType() == afl::net::redis::Key::Set);

    // Rename
    mock.expectCall("RENAME, k, u");
    mock.provideNewResult(new afl::data::IntegerValue(1));
    testee.renameTo("u");
    a.checkEqual("getName after renameTo", testee.getName(), "u");

    mock.expectCall("RENAMENX, u, v");
    mock.provideNewResult(new afl::data::IntegerValue(1));
    ok = testee.renameToUnique("v");
    a.check("renameToUnique ok", ok);
    a.checkEqual("getName after renameToUnique", testee.getName(), "v");

    // Remove
    mock.expectCall("DEL, v");
    mock.provideNewResult(new afl::data::IntegerValue(1));
    testee.remove();
}

/** Test Key against the InternalDatabase. */
AFL_TEST("afl.net.redis.Key:internal", a)
{
    afl::net::redis::InternalDatabase db;
    afl::net::redis::Key testee(db, "k");

    // Must create a set key
    afl::net::redis::IntegerSetKey(db, "k").add(9);

    // Local inquiry
    a.checkEqual("getName", testee.getName(), "k");
    a.checkEqual("getHandler", &testee.getHandler(), &db);

    // Network inquiry
    bool ok = testee.exists();
    a.check("exists ok", ok);
    a.check("getType", testee.getType() == afl::net::redis::Key::Set);

    // Rename
    testee.renameTo("u");
    a.checkEqual("getName after renameTo", testee.getName(), "u");

    ok = testee.renameToUnique("v");
    a.check("renameToUnique ok", ok);
    a.checkEqual("getName after renameToUnique", testee.getName(), "v");

    // Remove
    testee.remove();
}

/** Test types. */
AFL_TEST("afl.net.redis.Key:types", a)
{
    afl::net::redis::InternalDatabase db;

    // Define some values
    afl::net::redis::IntegerKey(db, "intKey").set(1);
    afl::net::redis::StringKey(db, "stringKey").set("a");
    afl::net::redis::IntegerSetKey(db, "intSetKey").add(42);
    afl::net::redis::StringSetKey(db, "stringSetKey").add("x");
    afl::net::redis::IntegerListKey(db, "intListKey").pushBack(23);
    afl::net::redis::StringListKey(db, "stringListKey").pushBack("y");
    afl::net::redis::HashKey(db, "hashKey").intField("f").set(9);

    // Inquiry
    using afl::net::redis::Key;
    a.checkEqual("getType int",         Key(db, "intKey").getType(), Key::String);
    a.checkEqual("getType string",      Key(db, "stringKey").getType(), Key::String);
    a.checkEqual("getType intSet",      Key(db, "intSetKey").getType(), Key::Set);
    a.checkEqual("getType stringSet",   Key(db, "stringSetKey").getType(), Key::Set);
    a.checkEqual("getType intList",     Key(db, "intListKey").getType(), Key::List);
    a.checkEqual("getType stringList",  Key(db, "stringListKey").getType(), Key::List);
    a.checkEqual("getType hash",        Key(db, "hashKey").getType(), Key::Hash);

    a.checkEqual("getType nonexistant", Key(db, "gibtsned").getType(), Key::None);

    // FIXME: ZSet is still missing
}
