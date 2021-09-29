/**
  *  \file u/t_net_redis_internaldatabase.cpp
  *  \brief Test for afl::net::redis::InternalDatabase
  */

#include <memory>
#include "afl/net/redis/internaldatabase.hpp"

#include "u/t_net_redis.hpp"
#include "afl/data/access.hpp"
#include "afl/data/segment.hpp"

using afl::net::redis::InternalDatabase;
using afl::data::Value;
using afl::data::Access;
using afl::data::Segment;

namespace {
    // Utility to quickly build a command
    class StringSegment : public Segment {
     public:
        StringSegment(const char* str);
        Segment& self()
            { return *this; }
    };

    // Given a list of integers, collect appropriate bits.
    int computeBits(Access a)
    {
        int bits = 0;
        for (size_t i = 0, n = a.getArraySize(); i < n; ++i) {
            int32_t v = a[i].toInteger();
            TS_ASSERT(v >= 0 && v <= 31);
            bits |= (1 << v);
        }
        return bits;
    }
}

StringSegment::StringSegment(const char* str)
{
    while (*str != '\0') {
        str += std::strspn(str, " ");
        size_t n = std::strcspn(str, " ");
        if (n == 0) {
            n = std::strlen(str);
        }
        if (n != 0) {
            pushBackString(String_t(str, n));
        }
        str += n;
    }
}



/** Test keyspace operations.
    - DEL
    - EXISTS
    - KEYS
    - RENAME
    - RENAMENX
    - TYPE */
void
TestNetRedisInternalDatabase::testKey()
{
    InternalDatabase db;
    std::auto_ptr<Value> result;

    // Check keyspace; must be empty
    result.reset(db.call(StringSegment("keys *").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 0U);

    // Create some keys
    db.callVoid(StringSegment("set ka a").self());
    db.callVoid(StringSegment("set kb b").self());
    db.callVoid(StringSegment("hset ha f v").self());

    // Check keyspace
    result.reset(db.call(StringSegment("keys *").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 3U);

    // Filter prefix
    result.reset(db.call(StringSegment("keys k*").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 2U);

    result.reset(db.call(StringSegment("keys k*a").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 1U);
    TS_ASSERT_EQUALS(Access(result)[0].toString(), "ka");

    result.reset(db.call(StringSegment("keys *a").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 2U);

    // Existence
    TS_ASSERT_EQUALS(db.callInt(StringSegment("exists ka").self()), 1);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("exists KA").self()), 0);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("exists ka ha").self()), 2);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("exists ka KA").self()), 1);

    // Deletion
    TS_ASSERT_EQUALS(db.callInt(StringSegment("del ka").self()), 1);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("exists ka").self()), 0);

    // Rename
    TS_ASSERT_THROWS_NOTHING(db.callVoid(StringSegment("rename kb ka").self()));

    // Rename again fails because the key is now missing
    TS_ASSERT_THROWS(db.callInt(StringSegment("rename kb ka").self()), std::exception);

    // Rename with existence check
    TS_ASSERT_EQUALS(db.callInt(StringSegment("renamenx ha ka").self()), 0);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("renamenx ha hb").self()), 1);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("exists ha").self()), 0);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("exists hb").self()), 1);

    // Type inquiry
    TS_ASSERT_EQUALS(db.callString(StringSegment("type ka").self()), "string");
    TS_ASSERT_EQUALS(db.callString(StringSegment("type hb").self()), "hash");
    TS_ASSERT_EQUALS(db.callString(StringSegment("type xxx").self()), "none");
}

/** Test string operations.
    - APPEND
    - GET
    - GETSET
    - MSET
    - SET
    - SETNX
    - STRLEN
    - GETRANGE */
void
TestNetRedisInternalDatabase::testString()
{
    InternalDatabase db;

    // Append, get, strlen
    TS_ASSERT_EQUALS(db.callInt(StringSegment("append s abc").self()), 3);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("append s mnop").self()), 7);
    TS_ASSERT_EQUALS(db.callString(StringSegment("get s").self()), "abcmnop");
    TS_ASSERT_EQUALS(db.callInt(StringSegment("strlen s").self()), 7);

    // Get non-existant
    TS_ASSERT_EQUALS(db.callString(StringSegment("get xx").self()), "");
    TS_ASSERT_EQUALS(db.callInt(StringSegment("strlen xx").self()), 0);

    // Set, getset
    db.callVoid(StringSegment("set a x").self());
    TS_ASSERT_EQUALS(db.callString(StringSegment("get a").self()), "x");
    TS_ASSERT_EQUALS(db.callString(StringSegment("getset a y").self()), "x");
    TS_ASSERT_EQUALS(db.callString(StringSegment("get a").self()), "y");

    // Set nonexistant
    TS_ASSERT_EQUALS(db.callInt(StringSegment("setnx a azz").self()), 0);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("setnx b bzz").self()), 1);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("setnx b czz").self()), 0);
    TS_ASSERT_EQUALS(db.callString(StringSegment("get b").self()), "bzz");

    // Wrong type
    db.callVoid(StringSegment("hset h k v").self());
    TS_ASSERT_THROWS(db.callVoid(StringSegment("append h x").self()), std::exception);
    TS_ASSERT_THROWS(db.callVoid(StringSegment("get h").self()), std::exception);
    TS_ASSERT_THROWS(db.callVoid(StringSegment("getset h x v").self()), std::exception);
    TS_ASSERT_THROWS(db.callVoid(StringSegment("strlen h").self()), std::exception);

    // Set can overwrite
    TS_ASSERT_EQUALS(db.callString(StringSegment("type h").self()), "hash");
    db.callVoid(StringSegment("set h x").self());
    TS_ASSERT_EQUALS(db.callString(StringSegment("type h").self()), "string");

    // Setting empty keeps it
    db.callVoid(StringSegment("set b").self().pushBackString(""));
    TS_ASSERT_EQUALS(db.callInt(StringSegment("exists b").self()), 1);

    // Multi-set
    db.callVoid(StringSegment("mset ka va kb vb kc vc").self());
    TS_ASSERT_EQUALS(db.callString(StringSegment("get ka").self()), "va");
    TS_ASSERT_EQUALS(db.callString(StringSegment("get kb").self()), "vb");
    TS_ASSERT_EQUALS(db.callString(StringSegment("get kc").self()), "vc");

    // Getrange
    db.callVoid(StringSegment("set k abcdefg").self());
    TS_ASSERT_EQUALS(db.callString(StringSegment("getrange k 0 0").self()), "a");
    TS_ASSERT_EQUALS(db.callString(StringSegment("getrange k 0 3").self()), "abcd");
    TS_ASSERT_EQUALS(db.callString(StringSegment("getrange k 3 3").self()), "d");
    TS_ASSERT_EQUALS(db.callString(StringSegment("getrange k -3 -3").self()), "e");
    TS_ASSERT_EQUALS(db.callString(StringSegment("getrange k 0 -3").self()), "abcde");
    TS_ASSERT_EQUALS(db.callString(StringSegment("getrange k -3 6").self()), "efg");
    TS_ASSERT_EQUALS(db.callString(StringSegment("getrange k -100 -200").self()), "a");
    TS_ASSERT_EQUALS(db.callString(StringSegment("getrange k 100 200").self()), "");
    TS_ASSERT_EQUALS(db.callString(StringSegment("getrange k 3 0").self()), "");
    TS_ASSERT_EQUALS(db.callString(StringSegment("getrange k 3 2").self()), "");
}

/** Test integer operations.
    - DECR
    - DECRBY
    - (GET)
    - INCR
    - INCRBY
    - (SET)
 */
void
TestNetRedisInternalDatabase::testInt()
{
    InternalDatabase db;

    // Get/set with integers (verifies that infrastructure converts correctly)
    db.callVoid(StringSegment("set i").self().pushBackInteger(12));
    TS_ASSERT_EQUALS(db.callInt(StringSegment("get i").self()), 12);

    // Arithmetic
    TS_ASSERT_EQUALS(db.callInt(StringSegment("incr i").self()),                        13);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("incrby i 5").self()),                    18);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("incrby i").self().pushBackInteger(99)), 117);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("incrby i -17").self()),                 100);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("incrby i +1").self()),                  101);

    TS_ASSERT_EQUALS(db.callInt(StringSegment("decr i").self()),                       100);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("decrby i 5").self()),                    95);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("decrby i").self().pushBackInteger(99)),  -4);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("decrby i -17").self()),                  13);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("decrby i +1").self()),                   12);

    // Create empty
    TS_ASSERT_EQUALS(db.callInt(StringSegment("incr e1").self()),                        1);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("incrby e2").self().pushBackInteger(9)),   9);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("decr e3").self()),                       -1);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("decrby e4").self().pushBackInteger(9)),  -9);

    // Wrong type or format
    db.callVoid(StringSegment("set i x").self());
    db.callVoid(StringSegment("hset h k v").self());
    TS_ASSERT_THROWS(db.callVoid(StringSegment("incr i").self()),                      std::exception);
    TS_ASSERT_THROWS(db.callVoid(StringSegment("incrby i").self().pushBackInteger(9)), std::exception);
    TS_ASSERT_THROWS(db.callVoid(StringSegment("decr i").self()),                      std::exception);
    TS_ASSERT_THROWS(db.callVoid(StringSegment("decrby i").self().pushBackInteger(9)), std::exception);
    TS_ASSERT_THROWS(db.callVoid(StringSegment("incr h").self()),                      std::exception);
    TS_ASSERT_THROWS(db.callVoid(StringSegment("incrby h").self().pushBackInteger(9)), std::exception);
    TS_ASSERT_THROWS(db.callVoid(StringSegment("decr h").self()),                      std::exception);
    TS_ASSERT_THROWS(db.callVoid(StringSegment("decrby h").self().pushBackInteger(9)), std::exception);
}

/** Test hash operations.
    - HDEL
    - HEXISTS
    - HGET
    - HINCRBY
    - HKEYS
    - HLEN
    - HMSET
    - HSET */
void
TestNetRedisInternalDatabase::testHash()
{
    InternalDatabase db;
    std::auto_ptr<Value> result;

    // Query empty hash
    TS_ASSERT_EQUALS(db.call   (StringSegment("hget k v").self()),    static_cast<Value*>(0));
    TS_ASSERT_EQUALS(db.callInt(StringSegment("hexists k v").self()), 0);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("hlen k").self()),      0);

    result.reset(db.call(StringSegment("hkeys k").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 0U);

    // Element access
    TS_ASSERT_EQUALS(db.callInt(StringSegment("hset k f v").self()), 1); /* 1=created */
    TS_ASSERT_EQUALS(db.callInt(StringSegment("hset k f v").self()), 0); /* 0=existed */
    TS_ASSERT_EQUALS(db.callInt(StringSegment("hset k f2 2").self()), 1); /* 1=created */

    TS_ASSERT_EQUALS(db.callString(StringSegment("hget k f").self()), "v");
    TS_ASSERT_EQUALS(db.callString(StringSegment("hget k f2").self()), "2");

    // Inquiry
    TS_ASSERT_EQUALS(db.callInt(StringSegment("hlen k").self()), 2);

    result.reset(db.call(StringSegment("hkeys k").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 2U);

    // Arithmetic
    TS_ASSERT_EQUALS(db.callInt(StringSegment("hincrby k f2").self().pushBackInteger(3)), 5);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("hincrby k f2 2").self()), 7);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("hincrby k f2 -10").self()), -3);

    // Field deletion
    TS_ASSERT_EQUALS(db.callInt(StringSegment("hdel k f").self()), 1); /* 1=deleted */
    TS_ASSERT_EQUALS(db.callInt(StringSegment("hdel k f").self()), 0); /* 0=did not exist */
    TS_ASSERT_EQUALS(db.callInt(StringSegment("exists k").self()), 1);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("hdel k f2").self()), 1); /* 1=deleted */

    // Final delete removed the hash
    TS_ASSERT_EQUALS(db.callInt(StringSegment("exists k").self()), 0);

    // Multi
    db.callVoid(StringSegment("hmset k f1 v1 f2 v2").self());
    TS_ASSERT_EQUALS(db.callInt(StringSegment("hlen k").self()), 2);
    TS_ASSERT_EQUALS(db.callString(StringSegment("hget k f2").self()), "v2");
}


/** Test list operations.
    - LINDEX
    - LLEN
    - LPOP
    - LPUSH
    - LRANGE
    - LREM
    - LSET
    - LTRIM
    - RPOP
    - RPOPLPUSH
    - RPUSH */
void
TestNetRedisInternalDatabase::testList()
{
    InternalDatabase db;
    std::auto_ptr<Value> result;

    // Create two lists
    //   left=[for,zre,two,one]
    //   right=[1,2,3,4]
    TS_ASSERT_EQUALS(db.callInt(StringSegment("lpush left one").self()), 1);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("lpush left two").self()), 2);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("lpush left zre").self()), 3);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("lpush left for").self()), 4);

    TS_ASSERT_EQUALS(db.callInt(StringSegment("rpush right 1").self()), 1);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("rpush right 2").self()), 2);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("rpush right 3").self()), 3);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("rpush right 4").self()), 4);

    // Inquiry
    TS_ASSERT_EQUALS(db.callInt(StringSegment("llen nx").self()), 0);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("llen left").self()), 4);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("llen right").self()), 4);

    result.reset(db.call(StringSegment("lrange left 0 -1").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 4U);
    TS_ASSERT_EQUALS(Access(result)[0].toString(), "for");
    TS_ASSERT_EQUALS(Access(result)[1].toString(), "zre");
    TS_ASSERT_EQUALS(Access(result)[2].toString(), "two");
    TS_ASSERT_EQUALS(Access(result)[3].toString(), "one");

    result.reset(db.call(StringSegment("lrange right 0 -1").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 4U);
    TS_ASSERT_EQUALS(Access(result)[0].toString(), "1");
    TS_ASSERT_EQUALS(Access(result)[1].toString(), "2");
    TS_ASSERT_EQUALS(Access(result)[2].toString(), "3");
    TS_ASSERT_EQUALS(Access(result)[3].toString(), "4");

    TS_ASSERT_EQUALS(db.callInt(StringSegment("lindex right").self().pushBackInteger(1)), 2);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("lindex right").self().pushBackInteger(-1)), 4);

    // Modification: cannot modify nonexistant
    TS_ASSERT_THROWS(db.callVoid(StringSegment("lset nx 0 a").self()), std::exception);

    // Modification: update left
    //   left=[four,three,two,one]
    TS_ASSERT_THROWS_NOTHING(db.callVoid(StringSegment("lset left 1 three").self()));
    TS_ASSERT_THROWS_NOTHING(db.callVoid(StringSegment("lset left").self().pushBackInteger(-4).pushBackString("four")));

    result.reset(db.call(StringSegment("lrange left 0 -1").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 4U);
    TS_ASSERT_EQUALS(Access(result)[0].toString(), "four");
    TS_ASSERT_EQUALS(Access(result)[1].toString(), "three");
    TS_ASSERT_EQUALS(Access(result)[2].toString(), "two");
    TS_ASSERT_EQUALS(Access(result)[3].toString(), "one");

    // More ranges
    result.reset(db.call(StringSegment("lrange left").self().pushBackInteger(3).pushBackInteger(2)));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 0U);

    result.reset(db.call(StringSegment("lrange left").self().pushBackInteger(2).pushBackInteger(2)));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 1U);
    TS_ASSERT_EQUALS(Access(result)[0].toString(), "two");

    result.reset(db.call(StringSegment("lrange left -2 2").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 1U);
    TS_ASSERT_EQUALS(Access(result)[0].toString(), "two");

    result.reset(db.call(StringSegment("lrange left").self().pushBackInteger(2).pushBackInteger(-2)));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 1U);
    TS_ASSERT_EQUALS(Access(result)[0].toString(), "two");

    result.reset(db.call(StringSegment("lrange left").self().pushBackInteger(3).pushBackInteger(-3)));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 0U);

    // Transfer: move one element
    //   left=[four,three,two]
    //   right=[one,1,2,3,4]
    TS_ASSERT_EQUALS(db.callString(StringSegment("rpoplpush left right").self()), "one");
    result.reset(db.call(StringSegment("lrange left 0 -1").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 3U);
    TS_ASSERT_EQUALS(Access(result)[0].toString(), "four");
    TS_ASSERT_EQUALS(Access(result)[1].toString(), "three");
    TS_ASSERT_EQUALS(Access(result)[2].toString(), "two");

    result.reset(db.call(StringSegment("lrange right 0 -1").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 5U);
    TS_ASSERT_EQUALS(Access(result)[0].toString(), "one");
    TS_ASSERT_EQUALS(Access(result)[1].toString(), "1");
    TS_ASSERT_EQUALS(Access(result)[2].toString(), "2");
    TS_ASSERT_EQUALS(Access(result)[3].toString(), "3");
    TS_ASSERT_EQUALS(Access(result)[4].toString(), "4");

    // Inplace transfer: move from one list to itself (=rotate)
    //   left=[two,four,three]
    TS_ASSERT_EQUALS(db.callString(StringSegment("rpoplpush left left").self()), "two");
    result.reset(db.call(StringSegment("lrange left 0 -1").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 3U);
    TS_ASSERT_EQUALS(Access(result)[0].toString(), "two");
    TS_ASSERT_EQUALS(Access(result)[1].toString(), "four");
    TS_ASSERT_EQUALS(Access(result)[2].toString(), "three");

    // Failure: cannot move from nonexistant list
    TS_ASSERT(!db.call(StringSegment("rpoplpush nx left").self()));

    // Creation: can move to nonexistant list
    //   left=[two,four]
    //   nx=[three]
    TS_ASSERT_EQUALS(db.callString(StringSegment("rpoplpush left nx").self()), "three");
    TS_ASSERT_EQUALS(db.callInt(StringSegment("llen nx").self()), 1);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("llen left").self()), 2);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("llen right").self()), 5);

    // Pop: make list empty
    //   nx=[] deletes it
    TS_ASSERT_EQUALS(db.callString(StringSegment("rpop nx").self()), "three");
    TS_ASSERT_EQUALS(db.callInt(StringSegment("exists nx").self()), 0);

    // Pop: nonempty list
    //   left=[four]
    TS_ASSERT_EQUALS(db.callString(StringSegment("lpop left").self()), "two");
    TS_ASSERT_EQUALS(db.callInt(StringSegment("exists left").self()), 1);

    // Pop: make list empty
    //   left=[] deletes it
    TS_ASSERT_EQUALS(db.callString(StringSegment("lpop left").self()), "four");
    TS_ASSERT_EQUALS(db.callInt(StringSegment("exists left").self()), 0);

    // Trim. At this point, right=[one,1,2,3,4].
    //   right=[1,2,3]
    db.callVoid(StringSegment("ltrim right").self().pushBackInteger(1).pushBackInteger(-2));
    TS_ASSERT_EQUALS(db.callInt(StringSegment("llen right").self()), 3);

    result.reset(db.call(StringSegment("lrange right 0 -1").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 3U);
    TS_ASSERT_EQUALS(Access(result)[0].toString(), "1");
    TS_ASSERT_EQUALS(Access(result)[1].toString(), "2");
    TS_ASSERT_EQUALS(Access(result)[2].toString(), "3");
}

/** Test set operations.
    - SADD
    - SCARD
    - SISMEMBER
    - SMEMBERS
    - SMOVE
    - SPOP
    - SRANDMEMBER
    - SREM */
void
TestNetRedisInternalDatabase::testSet()
{
    InternalDatabase db;
    std::auto_ptr<Value> result;

    // Create a set
    //   [1,2,3,4]
    TS_ASSERT_EQUALS(db.callInt(StringSegment("sadd s").self().pushBackInteger(1)), 1);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("sadd s").self().pushBackInteger(2)), 1);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("sadd s").self().pushBackInteger(3)), 1);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("sadd s").self().pushBackInteger(4)), 1);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("sadd s").self().pushBackInteger(4)), 0);

    // Verify
    TS_ASSERT_EQUALS(db.callInt(StringSegment("scard s").self()), 4);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("sismember s 3").self()), 1);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("sismember s 5").self()), 0);

    // Read out
    result.reset(db.call(StringSegment("smembers s").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 4U);
    TS_ASSERT_EQUALS(computeBits(result), (1<<1)|(1<<2)|(1<<3)|(1<<4));

    // Remove one
    //   [1,2,4]
    TS_ASSERT_EQUALS(db.callInt(StringSegment("srem s").self().pushBackInteger(3)), 1);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("srem s").self().pushBackInteger(3)), 0);
    result.reset(db.call(StringSegment("smembers s").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 3U);
    TS_ASSERT_EQUALS(computeBits(result), (1<<1)|(1<<2)|(1<<4));

    // Move
    //   s=[1,4]
    //   t=[2]
    TS_ASSERT_EQUALS(db.callInt(StringSegment("smove s t").self().pushBackInteger(2)), 1);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("smove s t").self().pushBackInteger(2)), 0);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("smove z t").self().pushBackInteger(2)), 0);

    result.reset(db.call(StringSegment("smembers s").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 2U);
    TS_ASSERT_EQUALS(computeBits(result), (1<<1)|(1<<4));

    result.reset(db.call(StringSegment("smembers t").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 1U);
    TS_ASSERT_EQUALS(computeBits(result), (1<<2));

    // Random from empty
    TS_ASSERT(!db.call(StringSegment("srandmember z").self()));
    TS_ASSERT(!db.call(StringSegment("spop z").self()));

    // Random from two-element set
    bool one = false;
    bool four = false;
    for (int i = 0; i < 100; ++i) {
        int32_t n = db.callInt(StringSegment("srandmember s").self());
        TS_ASSERT(n == 1 || n == 4);
        if (n == 1) {
            one = true;
        } else {
            four = true;
        }
    }
    TS_ASSERT(one);
    TS_ASSERT(four);

    // Remove from two-element set
    int32_t a = db.callInt(StringSegment("spop s").self());
    int32_t b = db.callInt(StringSegment("spop s").self());
    TS_ASSERT(a == 1 || a == 4);
    TS_ASSERT(b == 1 || b == 4);
    TS_ASSERT_DIFFERS(a, b);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("scard s").self()), 0);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("exists s").self()), 0);
}

/** Test set operations.
    - SDIFF
    - SDIFFSTORE
    - SINTER
    - SINTERSTORE
    - SUNION
    - SUNIONSTORE */
void
TestNetRedisInternalDatabase::testSetOps()
{
    InternalDatabase db;
    std::auto_ptr<Value> result;

    // Create some sets
    //   a=[1,2,3,4]
    //   b=[  2,  4,5,6]
    //   c=[        5]
    TS_ASSERT_EQUALS(db.callInt(StringSegment("sadd a").self().pushBackInteger(1).pushBackInteger(2).pushBackInteger(3).pushBackInteger(4)), 4);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("sadd b").self().pushBackInteger(2).pushBackInteger(4).pushBackInteger(5).pushBackInteger(6)), 4);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("sadd c").self().pushBackInteger(5)),                                                          1);

    // Set operations
    //   a-b = [1,3]
    //   b-a = [5,6]
    //   a+b = [1,2,3,4,5,6] = b+a
    //   a*b = [2,4] = b*a
    result.reset(db.call(StringSegment("sdiff a b").self()));
    TS_ASSERT_EQUALS(computeBits(result), (1<<1)|(1<<3));
    result.reset(db.call(StringSegment("sdiff b a").self()));
    TS_ASSERT_EQUALS(computeBits(result), (1<<5)|(1<<6));

    result.reset(db.call(StringSegment("sunion a b").self()));
    TS_ASSERT_EQUALS(computeBits(result), (1<<1)|(1<<2)|(1<<3)|(1<<4)|(1<<5)|(1<<6));
    result.reset(db.call(StringSegment("sunion b a").self()));
    TS_ASSERT_EQUALS(computeBits(result), (1<<1)|(1<<2)|(1<<3)|(1<<4)|(1<<5)|(1<<6));

    result.reset(db.call(StringSegment("sinter a b").self()));
    TS_ASSERT_EQUALS(computeBits(result), (1<<2)|(1<<4));
    result.reset(db.call(StringSegment("sinter b a").self()));
    TS_ASSERT_EQUALS(computeBits(result), (1<<2)|(1<<4));

    // With store, producing new element
    TS_ASSERT_EQUALS(db.callInt(StringSegment("sdiffstore z a b").self()), 2);
    result.reset(db.call(StringSegment("smembers z").self()));
    TS_ASSERT_EQUALS(computeBits(result), (1<<1)|(1<<3));

    // With store, overwriting existing element
    TS_ASSERT_EQUALS(db.callInt(StringSegment("sdiffstore z b a").self()), 2);
    result.reset(db.call(StringSegment("smembers z").self()));
    TS_ASSERT_EQUALS(computeBits(result), (1<<5)|(1<<6));

    // With store, empty result (deletes result key)
    TS_ASSERT_EQUALS(db.callInt(StringSegment("sinterstore z c a").self()), 0);
    result.reset(db.call(StringSegment("smembers z").self()));
    TS_ASSERT_EQUALS(computeBits(result), 0);
    TS_ASSERT_EQUALS(db.callInt(StringSegment("exists z").self()), 0);

    // With store, overwriting an input
    TS_ASSERT_EQUALS(db.callInt(StringSegment("sunionstore a b a").self()), 6);
    result.reset(db.call(StringSegment("smembers a").self()));
    TS_ASSERT_EQUALS(computeBits(result), (1<<1)|(1<<2)|(1<<3)|(1<<4)|(1<<5)|(1<<6));
}

/** Test sort operations with a list.
    - SORT */
void
TestNetRedisInternalDatabase::testSortList()
{
    InternalDatabase db;
    std::auto_ptr<Value> result;

    // Make a list [3,2,1,8,9,10,5]
    TS_ASSERT_EQUALS(db.callInt(StringSegment("rpush a").self().pushBackInteger(3).pushBackInteger(2).pushBackInteger(1).pushBackInteger(8).pushBackInteger(9).pushBackInteger(10).pushBackInteger(5)), 7);

    // Sort normally: [1,2,3,5,8,9,10]
    result.reset(db.call(StringSegment("sort a").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 7U);
    TS_ASSERT_EQUALS(Access(result)[0].toInteger(), 1);
    TS_ASSERT_EQUALS(Access(result)[1].toInteger(), 2);
    TS_ASSERT_EQUALS(Access(result)[2].toInteger(), 3);
    TS_ASSERT_EQUALS(Access(result)[3].toInteger(), 5);
    TS_ASSERT_EQUALS(Access(result)[4].toInteger(), 8);
    TS_ASSERT_EQUALS(Access(result)[5].toInteger(), 9);
    TS_ASSERT_EQUALS(Access(result)[6].toInteger(), 10);

    // Sort subrange: [3,5,8]
    result.reset(db.call(StringSegment("sort a limit 2 3").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 3U);
    TS_ASSERT_EQUALS(Access(result)[0].toInteger(), 3);
    TS_ASSERT_EQUALS(Access(result)[1].toInteger(), 5);
    TS_ASSERT_EQUALS(Access(result)[2].toInteger(), 8);

    // Sort descending: [10,9,8,5,3,2,1]
    result.reset(db.call(StringSegment("sort a desc").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 7U);
    TS_ASSERT_EQUALS(Access(result)[0].toInteger(), 10);
    TS_ASSERT_EQUALS(Access(result)[1].toInteger(), 9);
    TS_ASSERT_EQUALS(Access(result)[2].toInteger(), 8);
    TS_ASSERT_EQUALS(Access(result)[3].toInteger(), 5);
    TS_ASSERT_EQUALS(Access(result)[4].toInteger(), 3);
    TS_ASSERT_EQUALS(Access(result)[5].toInteger(), 2);
    TS_ASSERT_EQUALS(Access(result)[6].toInteger(), 1);

    // Sort lexically: [1,10,2,3,5,8,9]
    result.reset(db.call(StringSegment("sort a alpha").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 7U);
    TS_ASSERT_EQUALS(Access(result)[0].toInteger(), 1);
    TS_ASSERT_EQUALS(Access(result)[1].toInteger(), 10);
    TS_ASSERT_EQUALS(Access(result)[2].toInteger(), 2);
    TS_ASSERT_EQUALS(Access(result)[3].toInteger(), 3);
    TS_ASSERT_EQUALS(Access(result)[4].toInteger(), 5);
    TS_ASSERT_EQUALS(Access(result)[5].toInteger(), 8);
    TS_ASSERT_EQUALS(Access(result)[6].toInteger(), 9);

    // Sort non-existant
    result.reset(db.call(StringSegment("sort b").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 0U);

    // Sort non-existant with store
    db.callVoid(StringSegment("set c x").self());
    result.reset(db.call(StringSegment("sort b store c").self()));
    TS_ASSERT_EQUALS(db.callInt(StringSegment("exists c").self()), 0);

    // Sort with store
    db.callVoid(StringSegment("sort a store a").self());
    result.reset(db.call(StringSegment("lrange a").self().pushBackInteger(0).pushBackInteger(-1)));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 7U);
    TS_ASSERT_EQUALS(Access(result)[0].toInteger(), 1);
    TS_ASSERT_EQUALS(Access(result)[1].toInteger(), 2);
    TS_ASSERT_EQUALS(Access(result)[2].toInteger(), 3);
    TS_ASSERT_EQUALS(Access(result)[3].toInteger(), 5);
    TS_ASSERT_EQUALS(Access(result)[4].toInteger(), 8);
    TS_ASSERT_EQUALS(Access(result)[5].toInteger(), 9);
    TS_ASSERT_EQUALS(Access(result)[6].toInteger(), 10);
}

/** Test sort operations with a set.
    - SORT */
void
TestNetRedisInternalDatabase::testSortSet()
{
    InternalDatabase db;
    std::auto_ptr<Value> result;

    // Make a set [3,2,1,8,9,10,5]
    TS_ASSERT_EQUALS(db.callInt(StringSegment("sadd a").self().pushBackInteger(3).pushBackInteger(2).pushBackInteger(1).pushBackInteger(8).pushBackInteger(9).pushBackInteger(10).pushBackInteger(5)), 7);

    // Sort normally: [1,2,3,5,8,9,10]
    result.reset(db.call(StringSegment("sort a").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 7U);
    TS_ASSERT_EQUALS(Access(result)[0].toInteger(), 1);
    TS_ASSERT_EQUALS(Access(result)[1].toInteger(), 2);
    TS_ASSERT_EQUALS(Access(result)[2].toInteger(), 3);
    TS_ASSERT_EQUALS(Access(result)[3].toInteger(), 5);
    TS_ASSERT_EQUALS(Access(result)[4].toInteger(), 8);
    TS_ASSERT_EQUALS(Access(result)[5].toInteger(), 9);
    TS_ASSERT_EQUALS(Access(result)[6].toInteger(), 10);

    // Sort with store. This will turn the set into a list
    TS_ASSERT_EQUALS(db.callString(StringSegment("type a").self()), "set");
    db.callVoid(StringSegment("sort a store a").self());
    result.reset(db.call(StringSegment("lrange a").self().pushBackInteger(0).pushBackInteger(-1)));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 7U);
    TS_ASSERT_EQUALS(Access(result)[0].toInteger(), 1);
    TS_ASSERT_EQUALS(Access(result)[1].toInteger(), 2);
    TS_ASSERT_EQUALS(Access(result)[2].toInteger(), 3);
    TS_ASSERT_EQUALS(Access(result)[3].toInteger(), 5);
    TS_ASSERT_EQUALS(Access(result)[4].toInteger(), 8);
    TS_ASSERT_EQUALS(Access(result)[5].toInteger(), 9);
    TS_ASSERT_EQUALS(Access(result)[6].toInteger(), 10);
    TS_ASSERT_EQUALS(db.callString(StringSegment("type a").self()), "list");
}

/** Test sort operations with external keys and values.
    - SORT */
void
TestNetRedisInternalDatabase::testSortExt()
{
    InternalDatabase db;
    std::auto_ptr<Value> result;

    // Make a list
    db.callVoid(StringSegment("rpush a").self().pushBackInteger(3).pushBackInteger(1).pushBackInteger(2));

    // Make some keys
    db.callVoid(StringSegment("set k:1 one").self());
    db.callVoid(StringSegment("set k:2 two").self());
    db.callVoid(StringSegment("set k:3 three").self());

    // Make some keys in hashes
    db.callVoid(StringSegment("hset h:1 f un").self());
    db.callVoid(StringSegment("hset h:2 f dos").self());
    db.callVoid(StringSegment("hset h:3 f tres").self());

    // Sort by external key
    result.reset(db.call(StringSegment("sort a by k:* alpha").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 3U);
    TS_ASSERT_EQUALS(Access(result)[0].toInteger(), 1); // one
    TS_ASSERT_EQUALS(Access(result)[1].toInteger(), 3); // three
    TS_ASSERT_EQUALS(Access(result)[2].toInteger(), 2); // two

    // Sort by external key in hash
    result.reset(db.call(StringSegment("sort a by h:*->f alpha").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 3U);
    TS_ASSERT_EQUALS(Access(result)[0].toInteger(), 2); // dos
    TS_ASSERT_EQUALS(Access(result)[1].toInteger(), 3); // tres
    TS_ASSERT_EQUALS(Access(result)[2].toInteger(), 1); // un

    // Sort normally but get external key
    result.reset(db.call(StringSegment("sort a get k:* alpha").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 3U);
    TS_ASSERT_EQUALS(Access(result)[0].toString(), "one");
    TS_ASSERT_EQUALS(Access(result)[1].toString(), "two");
    TS_ASSERT_EQUALS(Access(result)[2].toString(), "three");

    // Sort normally but get two external keys
    result.reset(db.call(StringSegment("sort a get k:* get h:*->f alpha").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 6U);
    TS_ASSERT_EQUALS(Access(result)[0].toString(), "one");
    TS_ASSERT_EQUALS(Access(result)[1].toString(), "un");
    TS_ASSERT_EQUALS(Access(result)[2].toString(), "two");
    TS_ASSERT_EQUALS(Access(result)[3].toString(), "dos");
    TS_ASSERT_EQUALS(Access(result)[4].toString(), "three");
    TS_ASSERT_EQUALS(Access(result)[5].toString(), "tres");

    // Sort normally but get two external keys, with limit
    result.reset(db.call(StringSegment("sort a limit 1 1 get k:* get h:*->f alpha").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 2U);
    TS_ASSERT_EQUALS(Access(result)[0].toString(), "two");
    TS_ASSERT_EQUALS(Access(result)[1].toString(), "dos");

    // Sort by external key, get origin value and external key
    result.reset(db.call(StringSegment("sort a by k:* get h:*->f get # alpha").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 6U);
    TS_ASSERT_EQUALS(Access(result)[0].toString(), "un");
    TS_ASSERT_EQUALS(Access(result)[1].toString(), "1");
    TS_ASSERT_EQUALS(Access(result)[2].toString(), "tres");
    TS_ASSERT_EQUALS(Access(result)[3].toString(), "3");
    TS_ASSERT_EQUALS(Access(result)[4].toString(), "dos");
    TS_ASSERT_EQUALS(Access(result)[5].toString(), "2");

    // Sort normally, get two external keys, and store them in a list
    db.callVoid(StringSegment("sort a get k:* get h:*->f alpha store z").self());
    result.reset(db.call(StringSegment("lrange z").self().pushBackInteger(0).pushBackInteger(-1)));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 6U);
    TS_ASSERT_EQUALS(Access(result)[0].toString(), "one");
    TS_ASSERT_EQUALS(Access(result)[1].toString(), "un");
    TS_ASSERT_EQUALS(Access(result)[2].toString(), "two");
    TS_ASSERT_EQUALS(Access(result)[3].toString(), "dos");
    TS_ASSERT_EQUALS(Access(result)[4].toString(), "three");
    TS_ASSERT_EQUALS(Access(result)[5].toString(), "tres");

    // Null value: delete the external values for k:2 and h:3
    db.callVoid(StringSegment("del k:2 h:3").self());
    result.reset(db.call(StringSegment("sort a get k:* get h:*->f alpha").self()));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 6U);
    TS_ASSERT_EQUALS(Access(result)[0].toString(), "one");
    TS_ASSERT_EQUALS(Access(result)[1].toString(), "un");
    TS_ASSERT(Access(result)[2].isNull());
    TS_ASSERT_EQUALS(Access(result)[3].toString(), "dos");
    TS_ASSERT_EQUALS(Access(result)[4].toString(), "three");
    TS_ASSERT(Access(result)[5].isNull());

    // Same thing with store. Null values turn into empty strings here.
    db.callVoid(StringSegment("sort a get k:* get h:*->f alpha store z").self());
    result.reset(db.call(StringSegment("lrange z").self().pushBackInteger(0).pushBackInteger(-1)));
    TS_ASSERT_EQUALS(Access(result).getArraySize(), 6U);
    TS_ASSERT_EQUALS(Access(result)[0].toString(), "one");
    TS_ASSERT_EQUALS(Access(result)[1].toString(), "un");
    TS_ASSERT(!Access(result)[2].isNull());
    TS_ASSERT_EQUALS(Access(result)[2].toString(), "");
    TS_ASSERT_EQUALS(Access(result)[3].toString(), "dos");
    TS_ASSERT_EQUALS(Access(result)[4].toString(), "three");
    TS_ASSERT(!Access(result)[5].isNull());
    TS_ASSERT_EQUALS(Access(result)[5].toString(), "");
}
