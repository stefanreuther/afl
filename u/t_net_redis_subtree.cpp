/**
  *  \file u/t_net_redis_subtree.cpp
  *  \brief Test for afl::net::redis::Subtree
  */

#include "afl/net/redis/subtree.hpp"

#include "t_net_redis.hpp"
#include "afl/base/ptr.hpp"
#include "afl/data/stringvalue.hpp"
#include "afl/data/vector.hpp"
#include "afl/data/vectorvalue.hpp"
#include "afl/net/redis/internaldatabase.hpp"
#include "afl/test/commandhandler.hpp"

/** Test local operations (which do not have network communication). */
void
TestNetRedisSubtree::testLocal()
{
    afl::test::CommandHandler mock("testLocal");
    afl::net::redis::Subtree tree(mock, "x:");

    // Local
    TS_ASSERT_EQUALS(tree.getName(), "x:");
    TS_ASSERT_EQUALS(&tree.getHandler(), &mock);

    // Subkeys
    TS_ASSERT_EQUALS(tree.hashKey("y").getName(), "x:y");
    TS_ASSERT_EQUALS(tree.intKey("z").getName(), "x:z");
    TS_ASSERT_EQUALS(tree.stringKey("a").getName(), "x:a");
    TS_ASSERT_EQUALS(tree.intListKey("b").getName(), "x:b");
    TS_ASSERT_EQUALS(tree.stringListKey("c").getName(), "x:c");
    TS_ASSERT_EQUALS(tree.intSetKey("d").getName(), "x:d");
    TS_ASSERT_EQUALS(tree.stringSetKey("e").getName(), "x:e");
    TS_ASSERT_EQUALS(tree.key("f").getName(), "x:f");

    // Subtrees
    TS_ASSERT_EQUALS(tree.subtree("t").getName(), "x:t:");
    TS_ASSERT_EQUALS(tree.subtree("t").intKey("i").getName(), "x:t:i");
    TS_ASSERT_EQUALS(tree.subtree(99).getName(), "x:99:");
    TS_ASSERT_EQUALS(tree.subtree(99).stringKey("s").getName(), "x:99:s");
}

/** Test the getKeyNames operation. */
void
TestNetRedisSubtree::testKeys()
{
    // Test against the mock
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

        TS_ASSERT_EQUALS(list.size(), 3U);
        TS_ASSERT_EQUALS(list[0], "a");
        TS_ASSERT_EQUALS(list[1], "x:y");
        TS_ASSERT_EQUALS(list[2], "z");
    }

    // Test against InternalDatabase
    {
        afl::net::redis::InternalDatabase db;
        afl::net::redis::StringKey(db, "tree").set("a");
        afl::net::redis::StringKey(db, "tree:a").set("a");
        afl::net::redis::StringKey(db, "tree:x:y").set("a");
        afl::net::redis::StringKey(db, "tree:z").set("a");
        afl::net::redis::StringKey(db, "treeee").set("a");

        afl::data::StringList_t list;
        afl::net::redis::Subtree(db, "tree:").getKeyNames(list);

        TS_ASSERT_EQUALS(list.size(), 3U);
        TS_ASSERT_EQUALS(list[0], "a");
        TS_ASSERT_EQUALS(list[1], "x:y");
        TS_ASSERT_EQUALS(list[2], "z");

        // Another fetch appends
        afl::net::redis::Subtree(db, "tree:").getKeyNames(list);
        TS_ASSERT_EQUALS(list.size(), 6U);
    }
}
