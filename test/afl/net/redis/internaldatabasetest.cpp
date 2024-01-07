/**
  *  \file test/afl/net/redis/internaldatabasetest.cpp
  *  \brief Test for afl::net::redis::InternalDatabase
  */

#include "afl/net/redis/internaldatabase.hpp"

#include <memory>
#include "afl/data/access.hpp"
#include "afl/data/segment.hpp"
#include "afl/test/testrunner.hpp"

using afl::data::Access;
using afl::data::Segment;
using afl::data::Value;
using afl::net::redis::InternalDatabase;

namespace {
    // Utility to quickly build a command
    class StringSegment : public Segment {
     public:
        StringSegment(const char* str);
        Segment& self()
            { return *this; }
    };

    // Given a list of integers, collect appropriate bits.
    int computeBits(afl::test::Assert as, Access a)
    {
        int bits = 0;
        for (size_t i = 0, n = a.getArraySize(); i < n; ++i) {
            int32_t v = a[i].toInteger();
            as.check("valid value", v >= 0 && v <= 31);
            bits |= (1 << v);
        }
        return bits;
    }

    void checkBits(afl::test::Assert as, Access a, int32_t expectation)
    {
        as.checkEqual("result", computeBits(as, a), expectation);
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
AFL_TEST("afl.net.redis.InternalDatabase:key", a)
{
    InternalDatabase db;
    std::auto_ptr<Value> result;

    // Check keyspace; must be empty
    result.reset(db.call(StringSegment("keys *").self()));
    a.checkEqual("keys size 0", Access(result).getArraySize(), 0U);

    // Create some keys
    db.callVoid(StringSegment("set ka a").self());
    db.callVoid(StringSegment("set kb b").self());
    db.callVoid(StringSegment("hset ha f v").self());

    // Check keyspace
    result.reset(db.call(StringSegment("keys *").self()));
    a.checkEqual("keys size nonempty", Access(result).getArraySize(), 3U);

    // Filter prefix
    result.reset(db.call(StringSegment("keys k*").self()));
    a.checkEqual("keys prefix size", Access(result).getArraySize(), 2U);

    result.reset(db.call(StringSegment("keys k*a").self()));
    a.checkEqual("keys infix size", Access(result).getArraySize(), 1U);
    a.checkEqual("keys infix value", Access(result)[0].toString(), "ka");

    result.reset(db.call(StringSegment("keys *a").self()));
    a.checkEqual("keys suffix size", Access(result).getArraySize(), 2U);

    // Existence
    a.checkEqual("exists yes",   db.callInt(StringSegment("exists ka").self()), 1);
    a.checkEqual("exists no",    db.callInt(StringSegment("exists KA").self()), 0);
    a.checkEqual("exists 2*yes", db.callInt(StringSegment("exists ka ha").self()), 2);
    a.checkEqual("exists mixed", db.callInt(StringSegment("exists ka KA").self()), 1);

    // Deletion
    a.checkEqual("del", db.callInt(StringSegment("del ka").self()), 1);
    a.checkEqual("exists deleted", db.callInt(StringSegment("exists ka").self()), 0);

    // Rename
    AFL_CHECK_SUCCEEDS(a("rename"), db.callVoid(StringSegment("rename kb ka").self()));

    // Rename again fails because the key is now missing
    AFL_CHECK_THROWS(a("rename missing"), db.callInt(StringSegment("rename kb ka").self()), std::exception);

    // Rename with existence check
    a.checkEqual("renamenx existing",    db.callInt(StringSegment("renamenx ha ka").self()), 0);
    a.checkEqual("renamenx new",         db.callInt(StringSegment("renamenx ha hb").self()), 1);
    a.checkEqual("exists rename-source", db.callInt(StringSegment("exists ha").self()), 0);
    a.checkEqual("exists rename-target", db.callInt(StringSegment("exists hb").self()), 1);

    // Type inquiry
    a.checkEqual("type string", db.callString(StringSegment("type ka").self()), "string");
    a.checkEqual("type hash",   db.callString(StringSegment("type hb").self()), "hash");
    a.checkEqual("type none",   db.callString(StringSegment("type xxx").self()), "none");
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
AFL_TEST("afl.net.redis.InternalDatabase:string", a)
{
    InternalDatabase db;

    // Append, get, strlen
    a.checkEqual("01. append", db.callInt(StringSegment("append s abc").self()), 3);
    a.checkEqual("02. append", db.callInt(StringSegment("append s mnop").self()), 7);
    a.checkEqual("03. get",    db.callString(StringSegment("get s").self()), "abcmnop");
    a.checkEqual("04. strlen", db.callInt(StringSegment("strlen s").self()), 7);

    // Get non-existant
    a.checkEqual("11. get",    db.callString(StringSegment("get xx").self()), "");
    a.checkEqual("12. strlen", db.callInt(StringSegment("strlen xx").self()), 0);

    // Set, getset
    db.callVoid(StringSegment("set a x").self());
    a.checkEqual("21. get",    db.callString(StringSegment("get a").self()), "x");
    a.checkEqual("22. getset", db.callString(StringSegment("getset a y").self()), "x");
    a.checkEqual("23. get",    db.callString(StringSegment("get a").self()), "y");

    // Set nonexistant
    a.checkEqual("31. setnx", db.callInt(StringSegment("setnx a azz").self()), 0);
    a.checkEqual("32. setnx", db.callInt(StringSegment("setnx b bzz").self()), 1);
    a.checkEqual("33. setnx", db.callInt(StringSegment("setnx b czz").self()), 0);
    a.checkEqual("34. get ",  db.callString(StringSegment("get b").self()), "bzz");

    // Wrong type
    db.callVoid(StringSegment("hset h k v").self());
    AFL_CHECK_THROWS(a("41. append"), db.callVoid(StringSegment("append h x").self()), std::exception);
    AFL_CHECK_THROWS(a("42. get"),    db.callVoid(StringSegment("get h").self()), std::exception);
    AFL_CHECK_THROWS(a("43. getset"), db.callVoid(StringSegment("getset h x v").self()), std::exception);
    AFL_CHECK_THROWS(a("44. strlen"), db.callVoid(StringSegment("strlen h").self()), std::exception);

    // Set can overwrite
    a.checkEqual("51. type", db.callString(StringSegment("type h").self()), "hash");
    db.callVoid(StringSegment("set h x").self());
    a.checkEqual("52. type", db.callString(StringSegment("type h").self()), "string");

    // Setting empty keeps it
    db.callVoid(StringSegment("set b").self().pushBackString(""));
    a.checkEqual("61. set", db.callInt(StringSegment("exists b").self()), 1);

    // Multi-set
    db.callVoid(StringSegment("mset ka va kb vb kc vc").self());
    a.checkEqual("71. get", db.callString(StringSegment("get ka").self()), "va");
    a.checkEqual("72. get", db.callString(StringSegment("get kb").self()), "vb");
    a.checkEqual("73. get", db.callString(StringSegment("get kc").self()), "vc");

    // Getrange
    db.callVoid(StringSegment("set k abcdefg").self());
    a.checkEqual("81. getrange", db.callString(StringSegment("getrange k 0 0").self()), "a");
    a.checkEqual("82. getrange", db.callString(StringSegment("getrange k 0 3").self()), "abcd");
    a.checkEqual("83. getrange", db.callString(StringSegment("getrange k 3 3").self()), "d");
    a.checkEqual("84. getrange", db.callString(StringSegment("getrange k -3 -3").self()), "e");
    a.checkEqual("85. getrange", db.callString(StringSegment("getrange k 0 -3").self()), "abcde");
    a.checkEqual("86. getrange", db.callString(StringSegment("getrange k -3 6").self()), "efg");
    a.checkEqual("87. getrange", db.callString(StringSegment("getrange k -100 -200").self()), "a");
    a.checkEqual("88. getrange", db.callString(StringSegment("getrange k 100 200").self()), "");
    a.checkEqual("89. getrange", db.callString(StringSegment("getrange k 3 0").self()), "");
    a.checkEqual("8A. getrange", db.callString(StringSegment("getrange k 3 2").self()), "");
}

/** Test integer operations.
    - DECR
    - DECRBY
    - (GET)
    - INCR
    - INCRBY
    - (SET)
 */
AFL_TEST("afl.net.redis.InternalDatabase:int", a)
{
    InternalDatabase db;

    // Get/set with integers (verifies that infrastructure converts correctly)
    db.callVoid(StringSegment("set i").self().pushBackInteger(12));
    a.checkEqual("01. get", db.callInt(StringSegment("get i").self()), 12);

    // Arithmetic
    a.checkEqual("11. incr",   db.callInt(StringSegment("incr i").self()),                        13);
    a.checkEqual("12. incrby", db.callInt(StringSegment("incrby i 5").self()),                    18);
    a.checkEqual("13. incrby", db.callInt(StringSegment("incrby i").self().pushBackInteger(99)), 117);
    a.checkEqual("14. incrby", db.callInt(StringSegment("incrby i -17").self()),                 100);
    a.checkEqual("15. incrby", db.callInt(StringSegment("incrby i +1").self()),                  101);

    a.checkEqual("21. decr",   db.callInt(StringSegment("decr i").self()),                       100);
    a.checkEqual("22. decrby", db.callInt(StringSegment("decrby i 5").self()),                    95);
    a.checkEqual("23. decrby", db.callInt(StringSegment("decrby i").self().pushBackInteger(99)),  -4);
    a.checkEqual("24. decrby", db.callInt(StringSegment("decrby i -17").self()),                  13);
    a.checkEqual("25. decrby", db.callInt(StringSegment("decrby i +1").self()),                   12);

    // Create empty
    a.checkEqual("31. incr",   db.callInt(StringSegment("incr e1").self()),                        1);
    a.checkEqual("32. incrby", db.callInt(StringSegment("incrby e2").self().pushBackInteger(9)),   9);
    a.checkEqual("33. decr",   db.callInt(StringSegment("decr e3").self()),                       -1);
    a.checkEqual("34. decrby", db.callInt(StringSegment("decrby e4").self().pushBackInteger(9)),  -9);

    // Wrong type or format
    db.callVoid(StringSegment("set i x").self());
    db.callVoid(StringSegment("hset h k v").self());
    AFL_CHECK_THROWS(a("41. incr"),   db.callVoid(StringSegment("incr i").self()),                      std::exception);
    AFL_CHECK_THROWS(a("42. incrby"), db.callVoid(StringSegment("incrby i").self().pushBackInteger(9)), std::exception);
    AFL_CHECK_THROWS(a("43. decr"),   db.callVoid(StringSegment("decr i").self()),                      std::exception);
    AFL_CHECK_THROWS(a("44. decrby"), db.callVoid(StringSegment("decrby i").self().pushBackInteger(9)), std::exception);
    AFL_CHECK_THROWS(a("45. incr"),   db.callVoid(StringSegment("incr h").self()),                      std::exception);
    AFL_CHECK_THROWS(a("46. incrby"), db.callVoid(StringSegment("incrby h").self().pushBackInteger(9)), std::exception);
    AFL_CHECK_THROWS(a("47. decr"),   db.callVoid(StringSegment("decr h").self()),                      std::exception);
    AFL_CHECK_THROWS(a("48. decrby"), db.callVoid(StringSegment("decrby h").self().pushBackInteger(9)), std::exception);
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
AFL_TEST("afl.net.redis.InternalDatabase:hash", a)
{
    InternalDatabase db;
    std::auto_ptr<Value> result;

    // Query empty hash
    a.checkEqual("01. hget",    db.call   (StringSegment("hget k v").self()),    static_cast<Value*>(0));
    a.checkEqual("02. hexists", db.callInt(StringSegment("hexists k v").self()), 0);
    a.checkEqual("03. hlen",    db.callInt(StringSegment("hlen k").self()),      0);

    result.reset(db.call(StringSegment("hkeys k").self()));
    a.checkEqual("04. hkeys", Access(result).getArraySize(), 0U);

    // Element access
    a.checkEqual("11. hset", db.callInt(StringSegment("hset k f v").self()), 1); /* 1=created */
    a.checkEqual("12. hset", db.callInt(StringSegment("hset k f v").self()), 0); /* 0=existed */
    a.checkEqual("13. hset", db.callInt(StringSegment("hset k f2 2").self()), 1); /* 1=created */

    a.checkEqual("14. hget", db.callString(StringSegment("hget k f").self()), "v");
    a.checkEqual("15. hget", db.callString(StringSegment("hget k f2").self()), "2");

    // Inquiry
    a.checkEqual("21. hlen", db.callInt(StringSegment("hlen k").self()), 2);

    result.reset(db.call(StringSegment("hkeys k").self()));
    a.checkEqual("22. hkeys", Access(result).getArraySize(), 2U);

    // Arithmetic
    a.checkEqual("31. hincrby", db.callInt(StringSegment("hincrby k f2").self().pushBackInteger(3)), 5);
    a.checkEqual("32. hincrby", db.callInt(StringSegment("hincrby k f2 2").self()), 7);
    a.checkEqual("33. hincrby", db.callInt(StringSegment("hincrby k f2 -10").self()), -3);

    // Field deletion
    a.checkEqual("41. hdel",   db.callInt(StringSegment("hdel k f").self()), 1); /* 1=deleted */
    a.checkEqual("42. hdel",   db.callInt(StringSegment("hdel k f").self()), 0); /* 0=did not exist */
    a.checkEqual("43. exists", db.callInt(StringSegment("exists k").self()), 1);
    a.checkEqual("44. hdel",   db.callInt(StringSegment("hdel k f2").self()), 1); /* 1=deleted */

    // Final delete removed the hash
    a.checkEqual("51. exists", db.callInt(StringSegment("exists k").self()), 0);

    // Multi
    db.callVoid(StringSegment("hmset k f1 v1 f2 v2").self());
    a.checkEqual("61. hlen", db.callInt(StringSegment("hlen k").self()), 2);
    a.checkEqual("62. hget", db.callString(StringSegment("hget k f2").self()), "v2");
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
AFL_TEST("afl.net.redis.InternalDatabase:list", a)
{
    InternalDatabase db;
    std::auto_ptr<Value> result;

    // Create two lists
    //   left=[for,zre,two,one]
    //   right=[1,2,3,4]
    a.checkEqual("101. lpush", db.callInt(StringSegment("lpush left one").self()), 1);
    a.checkEqual("102. lpush", db.callInt(StringSegment("lpush left two").self()), 2);
    a.checkEqual("103. lpush", db.callInt(StringSegment("lpush left zre").self()), 3);
    a.checkEqual("104. lpush", db.callInt(StringSegment("lpush left for").self()), 4);

    a.checkEqual("105. rpush", db.callInt(StringSegment("rpush right 1").self()), 1);
    a.checkEqual("106. rpush", db.callInt(StringSegment("rpush right 2").self()), 2);
    a.checkEqual("107. rpush", db.callInt(StringSegment("rpush right 3").self()), 3);
    a.checkEqual("108. rpush", db.callInt(StringSegment("rpush right 4").self()), 4);

    // Inquiry
    a.checkEqual("111. llen", db.callInt(StringSegment("llen nx").self()), 0);
    a.checkEqual("112. llen", db.callInt(StringSegment("llen left").self()), 4);
    a.checkEqual("113. llen", db.callInt(StringSegment("llen right").self()), 4);

    result.reset(db.call(StringSegment("lrange left 0 -1").self()));
    a.checkEqual("121. lrange", Access(result).getArraySize(), 4U);
    a.checkEqual("122. lrange", Access(result)[0].toString(), "for");
    a.checkEqual("123. lrange", Access(result)[1].toString(), "zre");
    a.checkEqual("124. lrange", Access(result)[2].toString(), "two");
    a.checkEqual("125. lrange", Access(result)[3].toString(), "one");

    result.reset(db.call(StringSegment("lrange right 0 -1").self()));
    a.checkEqual("131. lrange", Access(result).getArraySize(), 4U);
    a.checkEqual("132. lrange", Access(result)[0].toString(), "1");
    a.checkEqual("133. lrange", Access(result)[1].toString(), "2");
    a.checkEqual("134. lrange", Access(result)[2].toString(), "3");
    a.checkEqual("135. lrange", Access(result)[3].toString(), "4");

    a.checkEqual("141. lindex", db.callInt(StringSegment("lindex right").self().pushBackInteger(1)), 2);
    a.checkEqual("142. lindex", db.callInt(StringSegment("lindex right").self().pushBackInteger(-1)), 4);

    // Modification: cannot modify nonexistant
    AFL_CHECK_THROWS(a("151. lset"), db.callVoid(StringSegment("lset nx 0 a").self()), std::exception);

    // Modification: update left
    //   left=[four,three,two,one]
    AFL_CHECK_SUCCEEDS(a("152. lset"), db.callVoid(StringSegment("lset left 1 three").self()));
    AFL_CHECK_SUCCEEDS(a("153. lset"), db.callVoid(StringSegment("lset left").self().pushBackInteger(-4).pushBackString("four")));

    result.reset(db.call(StringSegment("lrange left 0 -1").self()));
    a.checkEqual("161. lrange", Access(result).getArraySize(), 4U);
    a.checkEqual("162. lrange", Access(result)[0].toString(), "four");
    a.checkEqual("163. lrange", Access(result)[1].toString(), "three");
    a.checkEqual("164. lrange", Access(result)[2].toString(), "two");
    a.checkEqual("165. lrange", Access(result)[3].toString(), "one");

    // More ranges
    result.reset(db.call(StringSegment("lrange left").self().pushBackInteger(3).pushBackInteger(2)));
    a.checkEqual("171. lrange", Access(result).getArraySize(), 0U);

    result.reset(db.call(StringSegment("lrange left").self().pushBackInteger(2).pushBackInteger(2)));
    a.checkEqual("172. lrange", Access(result).getArraySize(), 1U);
    a.checkEqual("173. lrange", Access(result)[0].toString(), "two");

    result.reset(db.call(StringSegment("lrange left -2 2").self()));
    a.checkEqual("174. lrange", Access(result).getArraySize(), 1U);
    a.checkEqual("175. lrange", Access(result)[0].toString(), "two");

    result.reset(db.call(StringSegment("lrange left").self().pushBackInteger(2).pushBackInteger(-2)));
    a.checkEqual("176. lrange", Access(result).getArraySize(), 1U);
    a.checkEqual("177. lrange", Access(result)[0].toString(), "two");

    result.reset(db.call(StringSegment("lrange left").self().pushBackInteger(3).pushBackInteger(-3)));
    a.checkEqual("178. lrange", Access(result).getArraySize(), 0U);

    // Transfer: move one element
    //   left=[four,three,two]
    //   right=[one,1,2,3,4]
    a.checkEqual("181. rpoplpush", db.callString(StringSegment("rpoplpush left right").self()), "one");
    result.reset(db.call(StringSegment("lrange left 0 -1").self()));
    a.checkEqual("182. rpoplpush", Access(result).getArraySize(), 3U);
    a.checkEqual("183. rpoplpush", Access(result)[0].toString(), "four");
    a.checkEqual("184. rpoplpush", Access(result)[1].toString(), "three");
    a.checkEqual("185. rpoplpush", Access(result)[2].toString(), "two");

    result.reset(db.call(StringSegment("lrange right 0 -1").self()));
    a.checkEqual("191. lrange", Access(result).getArraySize(), 5U);
    a.checkEqual("192. lrange", Access(result)[0].toString(), "one");
    a.checkEqual("193. lrange", Access(result)[1].toString(), "1");
    a.checkEqual("194. lrange", Access(result)[2].toString(), "2");
    a.checkEqual("195. lrange", Access(result)[3].toString(), "3");
    a.checkEqual("196. lrange", Access(result)[4].toString(), "4");

    // Inplace transfer: move from one list to itself (=rotate)
    //   left=[two,four,three]
    a.checkEqual("201. rpoplpush", db.callString(StringSegment("rpoplpush left left").self()), "two");
    result.reset(db.call(StringSegment("lrange left 0 -1").self()));
    a.checkEqual("202. rpoplpush", Access(result).getArraySize(), 3U);
    a.checkEqual("203. rpoplpush", Access(result)[0].toString(), "two");
    a.checkEqual("204. rpoplpush", Access(result)[1].toString(), "four");
    a.checkEqual("205. rpoplpush", Access(result)[2].toString(), "three");

    // Failure: cannot move from nonexistant list
    a.check("211. rpoplpush", !db.call(StringSegment("rpoplpush nx left").self()));

    // Creation: can move to nonexistant list
    //   left=[two,four]
    //   nx=[three]
    a.checkEqual("221. rpoplpush", db.callString(StringSegment("rpoplpush left nx").self()), "three");
    a.checkEqual("222. rpoplpush", db.callInt(StringSegment("llen nx").self()), 1);
    a.checkEqual("223. rpoplpush", db.callInt(StringSegment("llen left").self()), 2);
    a.checkEqual("224. rpoplpush", db.callInt(StringSegment("llen right").self()), 5);

    // Pop: make list empty
    //   nx=[] deletes it
    a.checkEqual("231. rpop",   db.callString(StringSegment("rpop nx").self()), "three");
    a.checkEqual("232. exists", db.callInt(StringSegment("exists nx").self()), 0);

    // Pop: nonempty list
    //   left=[four]
    a.checkEqual("241. lpop",   db.callString(StringSegment("lpop left").self()), "two");
    a.checkEqual("242. exists", db.callInt(StringSegment("exists left").self()), 1);

    // Pop: make list empty
    //   left=[] deletes it
    a.checkEqual("251. lpop",   db.callString(StringSegment("lpop left").self()), "four");
    a.checkEqual("252. exists", db.callInt(StringSegment("exists left").self()), 0);

    // Trim. At this point, right=[one,1,2,3,4].
    //   right=[1,2,3]
    db.callVoid(StringSegment("ltrim right").self().pushBackInteger(1).pushBackInteger(-2));
    a.checkEqual("261. llen", db.callInt(StringSegment("llen right").self()), 3);

    result.reset(db.call(StringSegment("lrange right 0 -1").self()));
    a.checkEqual("271. lrange", Access(result).getArraySize(), 3U);
    a.checkEqual("272. lrange", Access(result)[0].toString(), "1");
    a.checkEqual("273. lrange", Access(result)[1].toString(), "2");
    a.checkEqual("274. lrange", Access(result)[2].toString(), "3");
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
AFL_TEST("afl.net.redis.InternalDatabase:set", a)
{
    InternalDatabase db;
    std::auto_ptr<Value> result;

    // Create a set
    //   [1,2,3,4]
    a.checkEqual("11. sadd", db.callInt(StringSegment("sadd s").self().pushBackInteger(1)), 1);
    a.checkEqual("12. sadd", db.callInt(StringSegment("sadd s").self().pushBackInteger(2)), 1);
    a.checkEqual("13. sadd", db.callInt(StringSegment("sadd s").self().pushBackInteger(3)), 1);
    a.checkEqual("14. sadd", db.callInt(StringSegment("sadd s").self().pushBackInteger(4)), 1);
    a.checkEqual("15. sadd", db.callInt(StringSegment("sadd s").self().pushBackInteger(4)), 0);

    // Verify
    a.checkEqual("21. scard",     db.callInt(StringSegment("scard s").self()), 4);
    a.checkEqual("22. sismember", db.callInt(StringSegment("sismember s 3").self()), 1);
    a.checkEqual("23. sismember", db.callInt(StringSegment("sismember s 5").self()), 0);

    // Read out
    result.reset(db.call(StringSegment("smembers s").self()));
    a.checkEqual("24. smembers", Access(result).getArraySize(), 4U);
    checkBits(a("25. smembers"), result, (1<<1)|(1<<2)|(1<<3)|(1<<4));

    // Remove one
    //   [1,2,4]
    a.checkEqual("31. srem", db.callInt(StringSegment("srem s").self().pushBackInteger(3)), 1);
    a.checkEqual("32. srem", db.callInt(StringSegment("srem s").self().pushBackInteger(3)), 0);
    result.reset(db.call(StringSegment("smembers s").self()));
    a.checkEqual("33. smembers", Access(result).getArraySize(), 3U);
    checkBits(a("34. smembers"), result, (1<<1)|(1<<2)|(1<<4));

    // Move
    //   s=[1,4]
    //   t=[2]
    a.checkEqual("41. smove", db.callInt(StringSegment("smove s t").self().pushBackInteger(2)), 1);
    a.checkEqual("42. smove", db.callInt(StringSegment("smove s t").self().pushBackInteger(2)), 0);
    a.checkEqual("43. smove", db.callInt(StringSegment("smove z t").self().pushBackInteger(2)), 0);

    result.reset(db.call(StringSegment("smembers s").self()));
    a.checkEqual("44. smembers", Access(result).getArraySize(), 2U);
    checkBits(a("45. smembers"), result, (1<<1)|(1<<4));

    result.reset(db.call(StringSegment("smembers t").self()));
    a.checkEqual("46. smembers", Access(result).getArraySize(), 1U);
    checkBits(a("47. smembers"), result, (1<<2));

    // Random from empty
    a.check("51. srandmember", !db.call(StringSegment("srandmember z").self()));
    a.check("52. spop", !db.call(StringSegment("spop z").self()));

    // Random from two-element set
    bool one = false;
    bool four = false;
    for (int i = 0; i < 100; ++i) {
        int32_t n = db.callInt(StringSegment("srandmember s").self());
        a.check("", n == 1 || n == 4);
        if (n == 1) {
            one = true;
        } else {
            four = true;
        }
    }
    a.check("53. srandmember: one", one);
    a.check("54. srandmember: four", four);

    // Remove from two-element set
    int32_t va = db.callInt(StringSegment("spop s").self());
    int32_t vb = db.callInt(StringSegment("spop s").self());
    a.check("61. spop", va == 1 || va == 4);
    a.check("62. spop", vb == 1 || vb == 4);
    a.checkDifferent("63. spop return", va, vb);
    a.checkEqual("64. scard", db.callInt(StringSegment("scard s").self()), 0);
    a.checkEqual("65. exists", db.callInt(StringSegment("exists s").self()), 0);
}

/** Test set operations.
    - SDIFF
    - SDIFFSTORE
    - SINTER
    - SINTERSTORE
    - SUNION
    - SUNIONSTORE */
AFL_TEST("afl.net.redis.InternalDatabase:set-ops", a)
{
    InternalDatabase db;
    std::auto_ptr<Value> result;

    // Create some sets
    //   a=[1,2,3,4]
    //   b=[  2,  4,5,6]
    //   c=[        5]
    a.checkEqual("01. sadd", db.callInt(StringSegment("sadd a").self().pushBackInteger(1).pushBackInteger(2).pushBackInteger(3).pushBackInteger(4)), 4);
    a.checkEqual("02. sadd", db.callInt(StringSegment("sadd b").self().pushBackInteger(2).pushBackInteger(4).pushBackInteger(5).pushBackInteger(6)), 4);
    a.checkEqual("03. sadd", db.callInt(StringSegment("sadd c").self().pushBackInteger(5)),                                                          1);

    // Set operations
    //   a-b = [1,3]
    //   b-a = [5,6]
    //   a+b = [1,2,3,4,5,6] = b+a
    //   a*b = [2,4] = b*a
    result.reset(db.call(StringSegment("sdiff a b").self()));
    checkBits(a("11. sdiff"), result, (1<<1)|(1<<3));
    result.reset(db.call(StringSegment("sdiff b a").self()));
    checkBits(a("12. sdiff"), result, (1<<5)|(1<<6));

    result.reset(db.call(StringSegment("sunion a b").self()));
    checkBits(a("13. sunion"), result, (1<<1)|(1<<2)|(1<<3)|(1<<4)|(1<<5)|(1<<6));
    result.reset(db.call(StringSegment("sunion b a").self()));
    checkBits(a("14. sunion"), result, (1<<1)|(1<<2)|(1<<3)|(1<<4)|(1<<5)|(1<<6));

    result.reset(db.call(StringSegment("sinter a b").self()));
    checkBits(a("15. sinter"), result, (1<<2)|(1<<4));
    result.reset(db.call(StringSegment("sinter b a").self()));
    checkBits(a("16. sinter"), result, (1<<2)|(1<<4));

    // With store, producing new element
    a.checkEqual("21. sdiffstore", db.callInt(StringSegment("sdiffstore z a b").self()), 2);
    result.reset(db.call(StringSegment("smembers z").self()));
    checkBits(a("22. smembers"), result, (1<<1)|(1<<3));

    // With store, overwriting existing element
    a.checkEqual("31. sdiffstore", db.callInt(StringSegment("sdiffstore z b a").self()), 2);
    result.reset(db.call(StringSegment("smembers z").self()));
    checkBits(a("32. smembers"), result, (1<<5)|(1<<6));

    // With store, empty result (deletes result key)
    a.checkEqual("41. sinterstore", db.callInt(StringSegment("sinterstore z c a").self()), 0);
    result.reset(db.call(StringSegment("smembers z").self()));
    checkBits(a("42. smembers"), result, 0);
    a.checkEqual("43. exists", db.callInt(StringSegment("exists z").self()), 0);

    // With store, overwriting an input
    a.checkEqual("51. sunionstore", db.callInt(StringSegment("sunionstore a b a").self()), 6);
    result.reset(db.call(StringSegment("smembers a").self()));
    checkBits(a("52. smembers"), result, (1<<1)|(1<<2)|(1<<3)|(1<<4)|(1<<5)|(1<<6));
}

/** Test sort operations with a list.
    - SORT */
AFL_TEST("afl.net.redis.InternalDatabase:sort:list", a)
{
    InternalDatabase db;
    std::auto_ptr<Value> result;

    // Make a list [3,2,1,8,9,10,5]
    a.checkEqual("01. rpush", db.callInt(StringSegment("rpush a").self().pushBackInteger(3).pushBackInteger(2).pushBackInteger(1).pushBackInteger(8).pushBackInteger(9).pushBackInteger(10).pushBackInteger(5)), 7);

    // Sort normally: [1,2,3,5,8,9,10]
    result.reset(db.call(StringSegment("sort a").self()));
    a.checkEqual("11. sort size", Access(result).getArraySize(), 7U);
    a.checkEqual("12. sort value", Access(result)[0].toInteger(), 1);
    a.checkEqual("13. sort value", Access(result)[1].toInteger(), 2);
    a.checkEqual("14. sort value", Access(result)[2].toInteger(), 3);
    a.checkEqual("15. sort value", Access(result)[3].toInteger(), 5);
    a.checkEqual("16. sort value", Access(result)[4].toInteger(), 8);
    a.checkEqual("17. sort value", Access(result)[5].toInteger(), 9);
    a.checkEqual("18. sort value", Access(result)[6].toInteger(), 10);

    // Sort subrange: [3,5,8]
    result.reset(db.call(StringSegment("sort a limit 2 3").self()));
    a.checkEqual("21. sort size", Access(result).getArraySize(), 3U);
    a.checkEqual("22. sort value", Access(result)[0].toInteger(), 3);
    a.checkEqual("23. sort value", Access(result)[1].toInteger(), 5);
    a.checkEqual("24. sort value", Access(result)[2].toInteger(), 8);

    // Sort descending: [10,9,8,5,3,2,1]
    result.reset(db.call(StringSegment("sort a desc").self()));
    a.checkEqual("31. sort size", Access(result).getArraySize(), 7U);
    a.checkEqual("32. sort value", Access(result)[0].toInteger(), 10);
    a.checkEqual("33. sort value", Access(result)[1].toInteger(), 9);
    a.checkEqual("34. sort value", Access(result)[2].toInteger(), 8);
    a.checkEqual("35. sort value", Access(result)[3].toInteger(), 5);
    a.checkEqual("36. sort value", Access(result)[4].toInteger(), 3);
    a.checkEqual("37. sort value", Access(result)[5].toInteger(), 2);
    a.checkEqual("38. sort value", Access(result)[6].toInteger(), 1);

    // Sort lexically: [1,10,2,3,5,8,9]
    result.reset(db.call(StringSegment("sort a alpha").self()));
    a.checkEqual("41. sort size", Access(result).getArraySize(), 7U);
    a.checkEqual("42. sort value", Access(result)[0].toInteger(), 1);
    a.checkEqual("43. sort value", Access(result)[1].toInteger(), 10);
    a.checkEqual("44. sort value", Access(result)[2].toInteger(), 2);
    a.checkEqual("45. sort value", Access(result)[3].toInteger(), 3);
    a.checkEqual("46. sort value", Access(result)[4].toInteger(), 5);
    a.checkEqual("47. sort value", Access(result)[5].toInteger(), 8);
    a.checkEqual("48. sort value", Access(result)[6].toInteger(), 9);

    // Sort non-existant
    result.reset(db.call(StringSegment("sort b").self()));
    a.checkEqual("51. sort", Access(result).getArraySize(), 0U);

    // Sort non-existant with store
    db.callVoid(StringSegment("set c x").self());
    result.reset(db.call(StringSegment("sort b store c").self()));
    a.checkEqual("52. exists", db.callInt(StringSegment("exists c").self()), 0);

    // Sort with store
    db.callVoid(StringSegment("sort a store a").self());
    result.reset(db.call(StringSegment("lrange a").self().pushBackInteger(0).pushBackInteger(-1)));
    a.checkEqual("61. lrange size", Access(result).getArraySize(), 7U);
    a.checkEqual("62. lrange value", Access(result)[0].toInteger(), 1);
    a.checkEqual("63. lrange value", Access(result)[1].toInteger(), 2);
    a.checkEqual("64. lrange value", Access(result)[2].toInteger(), 3);
    a.checkEqual("65. lrange value", Access(result)[3].toInteger(), 5);
    a.checkEqual("66. lrange value", Access(result)[4].toInteger(), 8);
    a.checkEqual("67. lrange value", Access(result)[5].toInteger(), 9);
    a.checkEqual("68. lrange value", Access(result)[6].toInteger(), 10);
}

/** Test sort operations with a set.
    - SORT */
AFL_TEST("afl.net.redis.InternalDatabase:sort:set", a)
{
    InternalDatabase db;
    std::auto_ptr<Value> result;

    // Make a set [3,2,1,8,9,10,5]
    a.checkEqual("01. sadd", db.callInt(StringSegment("sadd a").self().pushBackInteger(3).pushBackInteger(2).pushBackInteger(1).pushBackInteger(8).pushBackInteger(9).pushBackInteger(10).pushBackInteger(5)), 7);

    // Sort normally: [1,2,3,5,8,9,10]
    result.reset(db.call(StringSegment("sort a").self()));
    a.checkEqual("11. sort size", Access(result).getArraySize(), 7U);
    a.checkEqual("12. sort value", Access(result)[0].toInteger(), 1);
    a.checkEqual("13. sort value", Access(result)[1].toInteger(), 2);
    a.checkEqual("14. sort value", Access(result)[2].toInteger(), 3);
    a.checkEqual("15. sort value", Access(result)[3].toInteger(), 5);
    a.checkEqual("16. sort value", Access(result)[4].toInteger(), 8);
    a.checkEqual("17. sort value", Access(result)[5].toInteger(), 9);
    a.checkEqual("18. sort value", Access(result)[6].toInteger(), 10);

    // Sort with store. This will turn the set into a list
    a.checkEqual("21. type", db.callString(StringSegment("type a").self()), "set");
    db.callVoid(StringSegment("sort a store a").self());
    result.reset(db.call(StringSegment("lrange a").self().pushBackInteger(0).pushBackInteger(-1)));
    a.checkEqual("22. sort size", Access(result).getArraySize(), 7U);
    a.checkEqual("23. sort value", Access(result)[0].toInteger(), 1);
    a.checkEqual("24. sort value", Access(result)[1].toInteger(), 2);
    a.checkEqual("25. sort value", Access(result)[2].toInteger(), 3);
    a.checkEqual("26. sort value", Access(result)[3].toInteger(), 5);
    a.checkEqual("27. sort value", Access(result)[4].toInteger(), 8);
    a.checkEqual("28. sort value", Access(result)[5].toInteger(), 9);
    a.checkEqual("29. sort value", Access(result)[6].toInteger(), 10);
    a.checkEqual("2A. type", db.callString(StringSegment("type a").self()), "list");
}

/** Test sort operations with external keys and values.
    - SORT */
AFL_TEST("afl.net.redis.InternalDatabase:sort:external", a)
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
    a.checkEqual("01. sort size", Access(result).getArraySize(), 3U);
    a.checkEqual("02. sort value", Access(result)[0].toInteger(), 1); // one
    a.checkEqual("03. sort value", Access(result)[1].toInteger(), 3); // three
    a.checkEqual("04. sort value", Access(result)[2].toInteger(), 2); // two

    // Sort by external key in hash
    result.reset(db.call(StringSegment("sort a by h:*->f alpha").self()));
    a.checkEqual("11. sort size", Access(result).getArraySize(), 3U);
    a.checkEqual("12. sort value", Access(result)[0].toInteger(), 2); // dos
    a.checkEqual("13. sort value", Access(result)[1].toInteger(), 3); // tres
    a.checkEqual("14. sort value", Access(result)[2].toInteger(), 1); // un

    // Sort normally but get external key
    result.reset(db.call(StringSegment("sort a get k:* alpha").self()));
    a.checkEqual("21. sort size", Access(result).getArraySize(), 3U);
    a.checkEqual("22. sort value", Access(result)[0].toString(), "one");
    a.checkEqual("23. sort value", Access(result)[1].toString(), "two");
    a.checkEqual("24. sort value", Access(result)[2].toString(), "three");

    // Sort normally but get two external keys
    result.reset(db.call(StringSegment("sort a get k:* get h:*->f alpha").self()));
    a.checkEqual("31. sort size", Access(result).getArraySize(), 6U);
    a.checkEqual("32. sort value", Access(result)[0].toString(), "one");
    a.checkEqual("33. sort value", Access(result)[1].toString(), "un");
    a.checkEqual("34. sort value", Access(result)[2].toString(), "two");
    a.checkEqual("35. sort value", Access(result)[3].toString(), "dos");
    a.checkEqual("36. sort value", Access(result)[4].toString(), "three");
    a.checkEqual("37. sort value", Access(result)[5].toString(), "tres");

    // Sort normally but get two external keys, with limit
    result.reset(db.call(StringSegment("sort a limit 1 1 get k:* get h:*->f alpha").self()));
    a.checkEqual("41. sort size", Access(result).getArraySize(), 2U);
    a.checkEqual("42. sort value", Access(result)[0].toString(), "two");
    a.checkEqual("43. sort value", Access(result)[1].toString(), "dos");

    // Sort by external key, get origin value and external key
    result.reset(db.call(StringSegment("sort a by k:* get h:*->f get # alpha").self()));
    a.checkEqual("51. sort size", Access(result).getArraySize(), 6U);
    a.checkEqual("52. sort value", Access(result)[0].toString(), "un");
    a.checkEqual("53. sort value", Access(result)[1].toString(), "1");
    a.checkEqual("54. sort value", Access(result)[2].toString(), "tres");
    a.checkEqual("55. sort value", Access(result)[3].toString(), "3");
    a.checkEqual("56. sort value", Access(result)[4].toString(), "dos");
    a.checkEqual("57. sort value", Access(result)[5].toString(), "2");

    // Sort normally, get two external keys, and store them in a list
    db.callVoid(StringSegment("sort a get k:* get h:*->f alpha store z").self());
    result.reset(db.call(StringSegment("lrange z").self().pushBackInteger(0).pushBackInteger(-1)));
    a.checkEqual("61. sort size", Access(result).getArraySize(), 6U);
    a.checkEqual("62. sort value", Access(result)[0].toString(), "one");
    a.checkEqual("63. sort value", Access(result)[1].toString(), "un");
    a.checkEqual("64. sort value", Access(result)[2].toString(), "two");
    a.checkEqual("65. sort value", Access(result)[3].toString(), "dos");
    a.checkEqual("66. sort value", Access(result)[4].toString(), "three");
    a.checkEqual("67. sort value", Access(result)[5].toString(), "tres");

    // Null value: delete the external values for k:2 and h:3
    db.callVoid(StringSegment("del k:2 h:3").self());
    result.reset(db.call(StringSegment("sort a get k:* get h:*->f alpha").self()));
    a.checkEqual("71. sort size", Access(result).getArraySize(), 6U);
    a.checkEqual("72. sort value", Access(result)[0].toString(), "one");
    a.checkEqual("73. sort value", Access(result)[1].toString(), "un");
    a.check("73. sort value", Access(result)[2].isNull());
    a.checkEqual("74. sort value", Access(result)[3].toString(), "dos");
    a.checkEqual("75. sort value", Access(result)[4].toString(), "three");
    a.check("76. sort value", Access(result)[5].isNull());

    // Same thing with store. Null values turn into empty strings here.
    db.callVoid(StringSegment("sort a get k:* get h:*->f alpha store z").self());
    result.reset(db.call(StringSegment("lrange z").self().pushBackInteger(0).pushBackInteger(-1)));
    a.checkEqual("81. sort size", Access(result).getArraySize(), 6U);
    a.checkEqual("82. sort value", Access(result)[0].toString(), "one");
    a.checkEqual("83. sort value", Access(result)[1].toString(), "un");
    a.check("84. sort value", !Access(result)[2].isNull());
    a.checkEqual("85. sort value", Access(result)[2].toString(), "");
    a.checkEqual("86. sort value", Access(result)[3].toString(), "dos");
    a.checkEqual("87. sort value", Access(result)[4].toString(), "three");
    a.check("88. sort value", !Access(result)[5].isNull());
    a.checkEqual("89. sort value", Access(result)[5].toString(), "");
}
