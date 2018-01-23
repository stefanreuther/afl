/**
  *  \file u/t_net_redis_sortoperation.cpp
  *  \brief Test for afl::net::redis::SortOperation
  */

#include "afl/net/redis/sortoperation.hpp"

#include "t_net_redis.hpp"
#include "afl/data/access.hpp"
#include "afl/net/redis/hashkey.hpp"
#include "afl/net/redis/integerfield.hpp"
#include "afl/net/redis/internaldatabase.hpp"
#include "afl/net/redis/key.hpp"
#include "afl/net/redis/listkey.hpp"
#include "afl/net/redis/stringlistkey.hpp"
#include "afl/test/commandhandler.hpp"

/** Test against the mock. */
void
TestNetRedisSortOperation::testMock()
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
void
TestNetRedisSortOperation::testResults()
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
        TS_ASSERT_EQUALS(strList.size(), 5U);
        TS_ASSERT_EQUALS(strList[0], "1");
        TS_ASSERT_EQUALS(strList[1], "3");
        TS_ASSERT_EQUALS(strList[2], "5");
        TS_ASSERT_EQUALS(strList[3], "9");
        TS_ASSERT_EQUALS(strList[4], "12");
    }

    {
        afl::data::StringList_t strList;
        SortOperation(key).sortLexicographical().getResult(strList);
        TS_ASSERT_EQUALS(strList.size(), 5U);
        TS_ASSERT_EQUALS(strList[0], "1");
        TS_ASSERT_EQUALS(strList[1], "12");
        TS_ASSERT_EQUALS(strList[2], "3");
        TS_ASSERT_EQUALS(strList[3], "5");
        TS_ASSERT_EQUALS(strList[4], "9");
    }

    // Sort into integer list
    {
        afl::data::IntegerList_t intList;
        SortOperation(key).getResult(intList);
        TS_ASSERT_EQUALS(intList.size(), 5U);
        TS_ASSERT_EQUALS(intList[0], 1);
        TS_ASSERT_EQUALS(intList[1], 3);
        TS_ASSERT_EQUALS(intList[2], 5);
        TS_ASSERT_EQUALS(intList[3], 9);
        TS_ASSERT_EQUALS(intList[4], 12);
    }

    // Store into another key
    {
        afl::data::StringList_t strList;
        afl::net::redis::StringListKey result(db, "key");
        SortOperation(key).storeResult(result);
        result.getAll(strList);
        TS_ASSERT_EQUALS(strList.size(), 5U);
        TS_ASSERT_EQUALS(strList[0], "1");
        TS_ASSERT_EQUALS(strList[1], "3");
        TS_ASSERT_EQUALS(strList[2], "5");
        TS_ASSERT_EQUALS(strList[3], "9");
        TS_ASSERT_EQUALS(strList[4], "12");
    }

    // Get raw result
    {
        std::auto_ptr<afl::data::Value> result(SortOperation(key).getResult());
        TS_ASSERT(result.get() != 0);

        afl::data::Access a(result);
        TS_ASSERT_EQUALS(a.getArraySize(), 5U);
        TS_ASSERT_EQUALS(a[0].toInteger(), 1);
        TS_ASSERT_EQUALS(a[1].toInteger(), 3);
        TS_ASSERT_EQUALS(a[2].toInteger(), 5);
        TS_ASSERT_EQUALS(a[3].toInteger(), 9);
        TS_ASSERT_EQUALS(a[4].toInteger(), 12);
    }
}
