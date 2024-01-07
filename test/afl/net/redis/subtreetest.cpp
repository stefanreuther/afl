/**
  *  \file test/afl/net/redis/subtreetest.cpp
  *  \brief Test for afl::net::redis::Subtree
  */

#include "afl/net/redis/subtree.hpp"

#include "afl/data/stringvalue.hpp"
#include "afl/data/vector.hpp"
#include "afl/data/vectorvalue.hpp"
#include "afl/net/redis/internaldatabase.hpp"
#include "afl/test/commandhandler.hpp"
#include "afl/test/testrunner.hpp"

/** Test local operations (which do not have network communication). */
AFL_TEST("afl.net.redis.Subtree:local", a)
{
    afl::test::CommandHandler mock("testLocal");
    afl::net::redis::Subtree tree(mock, "x:");

    // Local
    a.checkEqual("getName", tree.getName(), "x:");
    a.checkEqual("getHandler", &tree.getHandler(), &mock);

    // Subkeys
    a.checkEqual("hashKey getName",       tree.hashKey("y").getName(), "x:y");
    a.checkEqual("intKey getName",        tree.intKey("z").getName(), "x:z");
    a.checkEqual("stringKey getName",     tree.stringKey("a").getName(), "x:a");
    a.checkEqual("intListKey getName",    tree.intListKey("b").getName(), "x:b");
    a.checkEqual("stringListKey getName", tree.stringListKey("c").getName(), "x:c");
    a.checkEqual("intSetKey getName",     tree.intSetKey("d").getName(), "x:d");
    a.checkEqual("stringSetKey getName",  tree.stringSetKey("e").getName(), "x:e");
    a.checkEqual("key getName",           tree.key("f").getName(), "x:f");

    // Subtrees
    a.checkEqual("subtree getName",               tree.subtree("t").getName(), "x:t:");
    a.checkEqual("subtree intKey getName",        tree.subtree("t").intKey("i").getName(), "x:t:i");
    a.checkEqual("subtree num getName",           tree.subtree(99).getName(), "x:99:");
    a.checkEqual("subtree num stringKey getName", tree.subtree(99).stringKey("s").getName(), "x:99:s");
}

/*
 *  Test the getKeyNames operation
 */

// Test against the mock
AFL_TEST("afl.net.redis.Subtree:getKeyNames:mock", a)
{
    afl::test::CommandHandler mock("testKeys");
    mock.expectCall("KEYS, tree:*");

    afl::base::Ref<afl::data::Vector> vec(afl::data::Vector::create());
    vec->pushBackNew(new afl::data::StringValue("tree:a"));
    vec->pushBackNew(new afl::data::StringValue("tree:x:y"));
    vec->pushBackNew(new afl::data::StringValue("tree:z"));
    mock.provideNewResult(new afl::data::VectorValue(vec));

    afl::data::StringList_t list;
    afl::net::redis::Subtree(mock, "tree:").getKeyNames(list);

    a.checkEqual("result size", list.size(), 3U);
    a.checkEqual("result 0", list[0], "a");
    a.checkEqual("result 1", list[1], "x:y");
    a.checkEqual("result 2", list[2], "z");
}

// Test against InternalDatabase
AFL_TEST("afl.net.redis.Subtree:getKeyNames:local", a)
{
    afl::net::redis::InternalDatabase db;
    afl::net::redis::StringKey(db, "tree").set("a");
    afl::net::redis::StringKey(db, "tree:a").set("a");
    afl::net::redis::StringKey(db, "tree:x:y").set("a");
    afl::net::redis::StringKey(db, "tree:z").set("a");
    afl::net::redis::StringKey(db, "treeee").set("a");

    afl::data::StringList_t list;
    afl::net::redis::Subtree(db, "tree:").getKeyNames(list);

    a.checkEqual("result size", list.size(), 3U);
    a.checkEqual("result 0", list[0], "a");
    a.checkEqual("result 1", list[1], "x:y");
    a.checkEqual("result 2", list[2], "z");

    // Another fetch appends
    afl::net::redis::Subtree(db, "tree:").getKeyNames(list);
    a.checkEqual("appended result", list.size(), 6U);
}
