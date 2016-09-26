/**
  *  \file u/t_string_formatunion.cpp
  *  \brief Test for afl::string::FormatUnion
  */

#include "afl/string/formatunion.hpp"
#include "u/t_string.hpp"

/** Simple test.
    We only test that we can create and use the object. */
void
TestStringFormatUnion::testIt()
{
    afl::string::FormatUnion u;
    u.m_ptr = 0;
    u.m_ptr = &u;

    u.m_signed = -1;
    TS_ASSERT(u.m_signed < 0);
    u.m_signed = +9999;
    TS_ASSERT(u.m_signed > 0);
    
    u.m_unsigned = static_cast<uintmax_t>(-1);
    TS_ASSERT(u.m_unsigned > 0);
    u.m_unsigned = 1;
    TS_ASSERT(u.m_unsigned > 0);
}
