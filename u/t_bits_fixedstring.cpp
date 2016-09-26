/**
  *  \file u/t_bits_fixedstring.cpp
  *  \brief Test for afl::bits::FixedString (+ functions)
  */

#include "afl/bits/fixedstring.hpp"

#include "u/t_bits.hpp"

/** Test unpackFixedString. */
void
TestBitsFixedString::testUnpack()
{
    using afl::base::ConstBytes_t;
    {
        static const uint8_t a[] = { 'a', 'b', 'c', ' ', ' ' };
        TS_ASSERT_EQUALS(afl::bits::unpackFixedString(ConstBytes_t(a)), "abc");
    }
    {
        static const uint8_t a[] = { 'a', 'b', 'c', 'd', 'e' };
        TS_ASSERT_EQUALS(afl::bits::unpackFixedString(ConstBytes_t(a)), "abcde");
    }
    {
        static const uint8_t a[] = { 'a', 'b', '\0', 'd', 'e' };
        TS_ASSERT_EQUALS(afl::bits::unpackFixedString(ConstBytes_t(a)), "ab");
    }
    {
        static const uint8_t a[] = { ' ', ' ', ' ', ' ', ' ' };
        TS_ASSERT_EQUALS(afl::bits::unpackFixedString(ConstBytes_t(a)), "");
    }
    {
        static const uint8_t a[] = { '\0', ' ', ' ', ' ', ' ' };
        TS_ASSERT_EQUALS(afl::bits::unpackFixedString(ConstBytes_t(a)), "");
    }
}

/** Test packFixedString. */
void
TestBitsFixedString::testPack()
{
    uint8_t tmp[5];
    using afl::base::Bytes_t;

    afl::bits::packFixedString(Bytes_t(tmp), afl::string::toMemory(""));
    TS_ASSERT_SAME_DATA(tmp, "     ", 5);

    afl::bits::packFixedString(Bytes_t(tmp), afl::string::toMemory("abc"));
    TS_ASSERT_SAME_DATA(tmp, "abc  ", 5);

    afl::bits::packFixedString(Bytes_t(tmp), afl::string::toMemory("abcde"));
    TS_ASSERT_SAME_DATA(tmp, "abcde", 5);

    afl::bits::packFixedString(Bytes_t(tmp), afl::string::toMemory("yxcvbnm"));
    TS_ASSERT_SAME_DATA(tmp, "yxcvb", 5);
}

/** Test the FixedString class. */
void
TestBitsFixedString::testClass()
{
    static const uint8_t a[3] = {'l','f','m'};
    typedef afl::bits::FixedString<3> FS_t;

    TS_ASSERT_EQUALS(FS_t::unpack(a), "lfm");

    uint8_t b[3];
    FS_t::pack(b, "lfm");
    TS_ASSERT_SAME_DATA(a, b, 3);
}
