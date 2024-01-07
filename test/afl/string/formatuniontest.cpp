/**
  *  \file test/afl/string/formatuniontest.cpp
  *  \brief Test for afl::string::FormatUnion
  */

#include "afl/string/formatunion.hpp"
#include "afl/test/testrunner.hpp"

/** Simple test.
    We only test that we can create and use the object. */
AFL_TEST("afl.string.FormatUnion", a)
{
    afl::string::FormatUnion u;
    u.m_ptr = 0;
    u.m_ptr = &u;

    u.m_signed = -1;
    a.check("01. signed", u.m_signed < 0);
    u.m_signed = +9999;
    a.check("02. signed", u.m_signed > 0);

    u.m_unsigned = static_cast<uintmax_t>(-1);
    a.check("03. unsigned", u.m_unsigned > 0);
    u.m_unsigned = 1;
    a.check("04. unsigned", u.m_unsigned > 0);
}
