/**
  *  \file u/t_charset_charset.cpp
  *  \brief Test for afl::charset::Charset
  */

#include "afl/charset/charset.hpp"

#include "u/t_charset.hpp"

/** Simple test. */
void
TestCharsetCharset::testIt()
{
    // Not much to test here; just test that the header file is valid.
    afl::charset::Charset* cs = 0;
    afl::base::Deletable* dd = cs;
    TS_ASSERT_EQUALS(dd, static_cast<afl::base::Deletable*>(0));
}

/** Interface test. */
void
TestCharsetCharset::testInterface()
{
    class Tester : public afl::charset::Charset {
     public:
        virtual String_t encode(afl::string::ConstStringMemory_t /*in*/)
            { return String_t(); }
        virtual String_t decode(afl::string::ConstStringMemory_t /*in*/)
            { return String_t(); }
        Tester* clone() const
            { return new Tester(); }
    };
    Tester t;
}
