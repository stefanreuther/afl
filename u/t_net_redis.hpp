/**
  *  \file u/t_net_redis.hpp
  *  \brief Test for afl::net::redis
  */
#ifndef AFL_U_T_NET_REDIS_HPP
#define AFL_U_T_NET_REDIS_HPP

#include <cxxtest/TestSuite.h>

class TestNetRedisIntegerKey : public CxxTest::TestSuite {
 public:
    void testMock();
    void testInternal();
};

class TestNetRedisIntegerSetKey : public CxxTest::TestSuite {
 public:
    void testMock();
    void testInternal();
};

class TestNetRedisInternalDatabase : public CxxTest::TestSuite {
 public:
    void testKey();
    void testString();
    void testInt();
    void testHash();
    void testList();
    void testSet();
    void testSetOps();
    void testSortList();
    void testSortSet();
    void testSortExt();
};

class TestNetRedisKey : public CxxTest::TestSuite {
 public:
    void testMock();
    void testInternal();
    void testTypes();
};

class TestNetRedisSortOperation : public CxxTest::TestSuite {
 public:
    void testMock();
    void testResults();
};

class TestNetRedisStringKey : public CxxTest::TestSuite {
 public:
    void testMock();
    void testInternal();
    void testSize();
};

class TestNetRedisStringSetKey : public CxxTest::TestSuite {
 public:
    void testMock();
    void testInternal();
};

class TestNetRedisSubtree : public CxxTest::TestSuite {
 public:
    void testLocal();
    void testKeys();
};

#endif
