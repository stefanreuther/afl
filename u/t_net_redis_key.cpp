/**
  *  \file u/t_net_redis_key.cpp
  *  \brief Test for afl::net::redis::Key
  */

#include "afl/net/redis/key.hpp"

#include "t_net_redis.hpp"
#include "u/mock/commandhandlermock.hpp"
#include "afl/data/integervalue.hpp"
#include "afl/data/stringvalue.hpp"
#include "afl/net/redis/integersetkey.hpp"
#include "afl/net/redis/internaldatabase.hpp"
#include "afl/net/redis/integerkey.hpp"
#include "afl/net/redis/stringkey.hpp"
#include "afl/net/redis/stringsetkey.hpp"
#include "afl/net/redis/integerlistkey.hpp"
#include "afl/net/redis/stringlistkey.hpp"
#include "afl/net/redis/hashkey.hpp"
#include "afl/net/redis/integerfield.hpp"

/** Test Key against the mock. */
void
TestNetRedisKey::testMock()
{
    CommandHandlerMock mock;
    afl::net::redis::Key testee(mock, "k");

    // Local inquiry
    TS_ASSERT_EQUALS(testee.getName(), "k");
    TS_ASSERT_EQUALS(&testee.getHandler(), &mock);

    // Network inquiry
    mock.addParameterList("EXISTS, k");
    mock.addNewResult(new afl::data::IntegerValue(1));
    bool ok = testee.exists();
    TS_ASSERT(ok);

    mock.addParameterList("TYPE, k");
    mock.addNewResult(new afl::data::StringValue("set"));
    TS_ASSERT_EQUALS(testee.getType(), afl::net::redis::Key::Set);

    // Rename
    mock.addParameterList("RENAME, k, u");
    mock.addNewResult(new afl::data::IntegerValue(1));
    testee.renameTo("u");
    TS_ASSERT_EQUALS(testee.getName(), "u");

    mock.addParameterList("RENAMENX, u, v");
    mock.addNewResult(new afl::data::IntegerValue(1));
    ok = testee.renameToUnique("v");
    TS_ASSERT(ok);
    TS_ASSERT_EQUALS(testee.getName(), "v");

    // Remove
    mock.addParameterList("DEL, v");
    mock.addNewResult(new afl::data::IntegerValue(1));
    testee.remove();
}

/** Test Key against the InternalDatabase. */
void
TestNetRedisKey::testInternal()
{
    afl::net::redis::InternalDatabase db;
    afl::net::redis::Key testee(db, "k");

    // Must create a set key
    afl::net::redis::IntegerSetKey(db, "k").add(9);

    // Local inquiry
    TS_ASSERT_EQUALS(testee.getName(), "k");
    TS_ASSERT_EQUALS(&testee.getHandler(), &db);

    // Network inquiry
    bool ok = testee.exists();
    TS_ASSERT(ok);
    TS_ASSERT_EQUALS(testee.getType(), afl::net::redis::Key::Set);

    // Rename
    testee.renameTo("u");
    TS_ASSERT_EQUALS(testee.getName(), "u");

    ok = testee.renameToUnique("v");
    TS_ASSERT(ok);
    TS_ASSERT_EQUALS(testee.getName(), "v");

    // Remove
    testee.remove();
}

/** Test types. */
void
TestNetRedisKey::testTypes()
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
    TS_ASSERT_EQUALS(Key(db, "intKey").getType(), Key::String);
    TS_ASSERT_EQUALS(Key(db, "stringKey").getType(), Key::String);
    TS_ASSERT_EQUALS(Key(db, "intSetKey").getType(), Key::Set);
    TS_ASSERT_EQUALS(Key(db, "stringSetKey").getType(), Key::Set);
    TS_ASSERT_EQUALS(Key(db, "intListKey").getType(), Key::List);
    TS_ASSERT_EQUALS(Key(db, "stringListKey").getType(), Key::List);
    TS_ASSERT_EQUALS(Key(db, "hashKey").getType(), Key::Hash);

    TS_ASSERT_EQUALS(Key(db, "gibtsned").getType(), Key::None);

    // FIXME: ZSet is still missing
}
