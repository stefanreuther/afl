/**
  *  \file test/afl/net/redis/sortoperationtest.cpp
  *  \brief Test for afl::net::redis::SortOperation
  */

#include "afl/net/redis/sortoperation.hpp"

#include "afl/data/access.hpp"
#include "afl/net/redis/hashkey.hpp"
#include "afl/net/redis/integerfield.hpp"
#include "afl/net/redis/internaldatabase.hpp"
#include "afl/net/redis/key.hpp"
#include "afl/net/redis/listkey.hpp"
#include "afl/net/redis/stringlistkey.hpp"
#include "afl/test/commandhandler.hpp"
#include "afl/test/testrunner.hpp"

/** Test against the mock. */
AFL_TEST_NOARG("afl.net.redis.SortOperation:mock")
{
    using afl::net::redis::SortOperation;

    // Key to work on
    afl::test::CommandHandler mock("testMock");
    afl::net::redis::Key key(mock, "key");
    afl::net::redis::ListKey result(mock, "result");

    // Sort by...
    mock.expectCall("SORT, key, BY, foo:*:bar, STORE, result");
    mock.provideNewResult(0);
    SortOperation(key).by("foo:*:bar").storeResult(result);

    mock.expectCall("SORT, key, BY, wild:*:card, STORE, result");
    mock.provideNewResult(0);
    SortOperation(key).by(afl::net::redis::Key(mock, "wild:*:card")).storeResult(result);

    mock.expectCall("SORT, key, BY, hash:*:foo->bar, STORE, result");
    mock.provideNewResult(0);
    SortOperation(key).by(afl::net::redis::HashKey(mock, "hash:*:foo").intField("bar")).storeResult(result);

    // Get...
    mock.expectCall("SORT, key, GET, #, STORE, result");
    mock.provideNewResult(0);
    SortOperation(key).get().storeResult(result);

    mock.expectCall("SORT, key, GET, a:*:b, STORE, result");
    mock.provideNewResult(0);
    SortOperation(key).get("a:*:b").storeResult(result);

    mock.expectCall("SORT, key, GET, wild:*:card, STORE, result");
    mock.provideNewResult(0);
    SortOperation(key).get(afl::net::redis::Key(mock, "wild:*:card")).storeResult(result);

    mock.expectCall("SORT, key, GET, hash:*:foo->bar, STORE, result");
    mock.provideNewResult(0);
    SortOperation(key).get(afl::net::redis::HashKey(mock, "hash:*:foo").intField("bar")).storeResult(result);

    mock.expectCall("SORT, key, GET, #, GET, wild:*:card, STORE, result");
    mock.provideNewResult(0);
    SortOperation(key).get().get(afl::net::redis::Key(mock, "wild:*:card")).storeResult(result);

    // Sort orders
    mock.expectCall("SORT, key, ALPHA, STORE, result");
    mock.provideNewResult(0);
    SortOperation(key).sortLexicographical().storeResult(result);

    mock.expectCall("SORT, key, DESC, STORE, result");
    mock.provideNewResult(0);
    SortOperation(key).sortReversed().storeResult(result);

    mock.expectCall("SORT, key, BY, nosort, STORE, result");
    mock.provideNewResult(0);
    SortOperation(key).sortDisable().storeResult(result);

    // Range
    mock.expectCall("SORT, key, LIMIT, 0, 10, STORE, result");
    mock.provideNewResult(0);
    SortOperation(key).limit(0, 10).storeResult(result);
}

/** Test various forms of results. */
AFL_TEST("afl.net.redis.SortOperation:results", a)
{
    using afl::net::redis::SortOperation;

    // Initialize DB
    afl::net::redis::InternalDatabase db;
    db.callVoid(afl::data::Segment().pushBackString("SADD").pushBackString("key").pushBackInteger(3).pushBackInteger(1).pushBackInteger(5).pushBackInteger(9).pushBackInteger(12));
    afl::net::redis::Key key(db, "key");

    // Sort into string list
    {
        afl::data::StringList_t strList;
        SortOperation(key).getResult(strList);
        a.checkEqual("01. size", strList.size(), 5U);
        a.checkEqual("02. value", strList[0], "1");
        a.checkEqual("03. value", strList[1], "3");
        a.checkEqual("04. value", strList[2], "5");
        a.checkEqual("05. value", strList[3], "9");
        a.checkEqual("06. value", strList[4], "12");
    }

    {
        afl::data::StringList_t strList;
        SortOperation(key).sortLexicographical().getResult(strList);
        a.checkEqual("11. size", strList.size(), 5U);
        a.checkEqual("12. value", strList[0], "1");
        a.checkEqual("13. value", strList[1], "12");
        a.checkEqual("14. value", strList[2], "3");
        a.checkEqual("15. value", strList[3], "5");
        a.checkEqual("16. value", strList[4], "9");
    }

    // Sort into integer list
    {
        afl::data::IntegerList_t intList;
        SortOperation(key).getResult(intList);
        a.checkEqual("21. size", intList.size(), 5U);
        a.checkEqual("22. value", intList[0], 1);
        a.checkEqual("23. value", intList[1], 3);
        a.checkEqual("24. value", intList[2], 5);
        a.checkEqual("25. value", intList[3], 9);
        a.checkEqual("26. value", intList[4], 12);
    }

    // Store into another key
    {
        afl::data::StringList_t strList;
        afl::net::redis::StringListKey result(db, "key");
        SortOperation(key).storeResult(result);
        result.getAll(strList);
        a.checkEqual("31. size", strList.size(), 5U);
        a.checkEqual("32. value", strList[0], "1");
        a.checkEqual("33. value", strList[1], "3");
        a.checkEqual("34. value", strList[2], "5");
        a.checkEqual("35. value", strList[3], "9");
        a.checkEqual("36. value", strList[4], "12");
    }

    // Get raw result
    {
        std::auto_ptr<afl::data::Value> result(SortOperation(key).getResult());
        a.checkNonNull("41. result", result.get());

        afl::data::Access aa(result);
        a.checkEqual("42. size", aa.getArraySize(), 5U);
        a.checkEqual("43. value", aa[0].toInteger(), 1);
        a.checkEqual("44. value", aa[1].toInteger(), 3);
        a.checkEqual("45. value", aa[2].toInteger(), 5);
        a.checkEqual("46. value", aa[3].toInteger(), 9);
        a.checkEqual("47. value", aa[4].toInteger(), 12);
    }
}
