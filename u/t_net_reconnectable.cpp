/**
  *  \file u/t_net_reconnectable.cpp
  *  \brief Test for afl::net::Reconnectable
  */

#include "afl/net/reconnectable.hpp"

#include "t_net.hpp"

/** Interface test. */
void
TestNetReconnectable::testInterface()
{
    class Tester : public afl::net::Reconnectable {
     public:
        virtual void setReconnectMode(Mode /*mode*/)
            { }
    };
    Tester t;
}
