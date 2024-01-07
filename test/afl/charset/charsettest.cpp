/**
  *  \file test/afl/charset/charsettest.cpp
  *  \brief Test for afl::charset::Charset
  */

#include "afl/charset/charset.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.charset.Charset:types", a)
{
    afl::charset::Charset* cs = 0;
    afl::base::Deletable* dd = cs;
    a.checkNull("", dd);
}

AFL_TEST_NOARG("afl.charset.Charset:interface")
{
    class Tester : public afl::charset::Charset {
     public:
        virtual afl::base::GrowableBytes_t encode(afl::string::ConstStringMemory_t /*in*/)
            { return afl::base::GrowableBytes_t(); }
        virtual String_t decode(afl::base::ConstBytes_t /*in*/)
            { return String_t(); }
        Tester* clone() const
            { return new Tester(); }
    };
    Tester t;
}
