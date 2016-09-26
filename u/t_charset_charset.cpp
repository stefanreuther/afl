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
