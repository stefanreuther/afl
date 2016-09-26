/**
  *  \file u/t_net_redis.hpp
  *  \brief Test for afl::net::redis
  */
#ifndef AFL_U_T_NET_REDIS_HPP
#define AFL_U_T_NET_REDIS_HPP

#include <cxxtest/TestSuite.h>

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

#endif
