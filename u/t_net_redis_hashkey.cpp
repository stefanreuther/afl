/**
  *  \file u/t_net_redis_hashkey.cpp
  *  \brief Test for afl::net::redis::HashKey
  */

#include <algorithm>
#include "afl/net/redis/hashkey.hpp"

#include "t_net_redis.hpp"
#include "afl/net/redis/integerfield.hpp"
#include "afl/net/redis/internaldatabase.hpp"
#include "afl/net/redis/stringfield.hpp"

void
TestNetRedisHashKey::testIt()
{
    afl::net::redis::InternalDatabase db;
    afl::net::redis::HashKey testee(db, "k");

    TS_ASSERT_EQUALS(testee.size(), 0);

    // Set some fields
    testee.intField("i").set(3);
    testee.stringField("a").set("b");

    // Verify fields have been set
    afl::data::StringList_t fieldNames;
    testee.getFieldNames(fieldNames);
    std::sort(fieldNames.begin(), fieldNames.end());
    TS_ASSERT_EQUALS(fieldNames.size(), 2U);
    TS_ASSERT_EQUALS(fieldNames[0], "a");
    TS_ASSERT_EQUALS(fieldNames[1], "i");

    TS_ASSERT_EQUALS(testee.size(), 2);
    TS_ASSERT(testee.field("a").exists());
    TS_ASSERT(testee.field("i").exists());

    // Get all fields
    afl::data::StringList_t fieldValues;
    testee.getAll(fieldValues);
    TS_ASSERT_EQUALS(fieldValues.size(), 4U);
    if (fieldValues[0] == "a") {
        TS_ASSERT_EQUALS(fieldValues[0], "a");
        TS_ASSERT_EQUALS(fieldValues[1], "b");
        TS_ASSERT_EQUALS(fieldValues[2], "i");
        TS_ASSERT_EQUALS(fieldValues[3], "3");
    } else {
        TS_ASSERT_EQUALS(fieldValues[0], "i");
        TS_ASSERT_EQUALS(fieldValues[1], "3");
        TS_ASSERT_EQUALS(fieldValues[2], "a");
        TS_ASSERT_EQUALS(fieldValues[3], "b");
    }

    // Set all
    afl::data::StringList_t newValues;
    newValues.push_back("a");
    newValues.push_back("x");
    newValues.push_back("y");
    newValues.push_back("z");
    testee.setAll(newValues);

    TS_ASSERT_EQUALS(testee.stringField("a").get(), "x");
    TS_ASSERT_EQUALS(testee.stringField("y").get(), "z");
    TS_ASSERT_EQUALS(testee.stringField("i").get(), "3");

    // It is not an error to use an empty list
    afl::data::StringList_t empty;
    testee.setAll(empty);
}

