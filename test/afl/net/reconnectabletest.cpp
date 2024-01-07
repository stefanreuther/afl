/**
  *  \file test/afl/net/reconnectabletest.cpp
  *  \brief Test for afl::net::Reconnectable
  */

#include "afl/net/reconnectable.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST_NOARG("afl.net.Reconnectable")
{
    class Tester : public afl::net::Reconnectable {
     public:
        virtual void setReconnectMode(Mode /*mode*/)
            { }
    };
    Tester t;
}
