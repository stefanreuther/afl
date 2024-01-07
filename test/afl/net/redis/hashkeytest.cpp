/**
  *  \file test/afl/net/redis/hashkeytest.cpp
  *  \brief Test for afl::net::redis::HashKey
  */

#include "afl/net/redis/hashkey.hpp"

#include <algorithm>
#include "afl/net/redis/integerfield.hpp"
#include "afl/net/redis/internaldatabase.hpp"
#include "afl/net/redis/stringfield.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.net.redis.HashKey", a)
{
    afl::net::redis::InternalDatabase db;
    afl::net::redis::HashKey testee(db, "k");

    a.checkEqual("01. size", testee.size(), 0);

    // Set some fields
    testee.intField("i").set(3);
    testee.stringField("a").set("b");

    // Verify fields have been set
    afl::data::StringList_t fieldNames;
    testee.getFieldNames(fieldNames);
    std::sort(fieldNames.begin(), fieldNames.end());
    a.checkEqual("11. fieldNames", fieldNames.size(), 2U);
    a.checkEqual("12. fieldName a", fieldNames[0], "a");
    a.checkEqual("13. fieldName i", fieldNames[1], "i");

    a.checkEqual("21. size", testee.size(), 2);
    a.check("22. field exists", testee.field("a").exists());
    a.check("23. field exists", testee.field("i").exists());

    // Get all fields
    afl::data::StringList_t fieldValues;
    testee.getAll(fieldValues);
    a.checkEqual("31. fieldValues", fieldValues.size(), 4U);
    if (fieldValues[0] == "a") {
        a.checkEqual("32. fieldValues", fieldValues[0], "a");
        a.checkEqual("33. fieldValues", fieldValues[1], "b");
        a.checkEqual("34. fieldValues", fieldValues[2], "i");
        a.checkEqual("35. fieldValues", fieldValues[3], "3");
    } else {
        a.checkEqual("36. fieldValues", fieldValues[0], "i");
        a.checkEqual("37. fieldValues", fieldValues[1], "3");
        a.checkEqual("38. fieldValues", fieldValues[2], "a");
        a.checkEqual("39. fieldValues", fieldValues[3], "b");
    }

    // Set all
    afl::data::StringList_t newValues;
    newValues.push_back("a");
    newValues.push_back("x");
    newValues.push_back("y");
    newValues.push_back("z");
    testee.setAll(newValues);

    a.checkEqual("41. field a", testee.stringField("a").get(), "x");
    a.checkEqual("42. field y", testee.stringField("y").get(), "z");
    a.checkEqual("23. field i", testee.stringField("i").get(), "3");

    // It is not an error to use an empty list
    afl::data::StringList_t empty;
    testee.setAll(empty);
}
