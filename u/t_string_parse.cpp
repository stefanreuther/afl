/**
  *  \file u/t_string_parse.cpp
  *  \brief Test for afl/string/parse.hpp
  */

#include "afl/string/parse.hpp"

#include "u/t_string.hpp"

/** Test strToInteger. */
void
TestStringParse::testStrToInt()
{
    using afl::string::strToInteger;

    String_t a = "126";
    String_t b = "42km";
    String_t c = "45 ";
    String_t d = " 17 ";
    String_t e = "   99  miles ";
    int res;
    String_t::size_type pos;

    TS_ASSERT(strToInteger(a, res));
    TS_ASSERT_EQUALS(res, 126);

    TS_ASSERT(!strToInteger(b, res));
    TS_ASSERT(!strToInteger(b, res, pos));
    TS_ASSERT_EQUALS(pos, 2U);
    TS_ASSERT(strToInteger(b.substr(0, pos), res));
    TS_ASSERT_EQUALS(res, 42);

    TS_ASSERT(strToInteger(c, res));
    TS_ASSERT_EQUALS(res, 45);

    TS_ASSERT(strToInteger(d, res));
    TS_ASSERT_EQUALS(res, 17);

    TS_ASSERT(!strToInteger(e, res));
    TS_ASSERT(!strToInteger(e, res, pos));
    TS_ASSERT_EQUALS(pos, 7U);
    TS_ASSERT(strToInteger(e.substr(0, pos), res));
    TS_ASSERT_EQUALS(res, 99);

    // Border cases
    TS_ASSERT(!strToInteger("", res));
    TS_ASSERT(!strToInteger(" ", res));

    // Now with overflow checking
    String_t f = "240";
    int8_t i8 = 0;
    uint8_t u8 = 0;
    long il = 0;
    unsigned long ul = 0;
    TS_ASSERT(!strToInteger(f, i8));
    TS_ASSERT(strToInteger(f, u8));
    TS_ASSERT_EQUALS(u8, 240U);

    String_t t = "1000";
    TS_ASSERT(!strToInteger(t, i8));
    TS_ASSERT(!strToInteger(t, u8));

    // Negative
    String_t g = "-10";
    TS_ASSERT(strToInteger(g, i8));
    TS_ASSERT(!strToInteger(g, u8));
    TS_ASSERT_EQUALS(i8, -10);

    TS_ASSERT(strToInteger(g, il));
    TS_ASSERT(!strToInteger(g, ul));
    TS_ASSERT_EQUALS(il, -10);
}

/** Test conversion to long long. */
void
TestStringParse::testStrToLL()
{
    using afl::string::strToInteger;

    uint64_t u64;
    int64_t i64;
    unsigned long long ull;
    long long ll;

    TS_ASSERT(strToInteger("1", u64));
    TS_ASSERT_EQUALS(u64, 1U);
    TS_ASSERT(strToInteger("1", i64));
    TS_ASSERT_EQUALS(i64, 1);
    TS_ASSERT(strToInteger("1", ull));
    TS_ASSERT_EQUALS(ull, 1U);
    TS_ASSERT(strToInteger("1", ll));
    TS_ASSERT_EQUALS(ll, 1);

    TS_ASSERT(strToInteger("123456789101112", u64));
    TS_ASSERT_EQUALS(u64, 123456789101112ULL);
    TS_ASSERT(strToInteger("123456789101112", i64));
    TS_ASSERT_EQUALS(i64, 123456789101112LL);
    TS_ASSERT(strToInteger("123456789101112", ull));
    TS_ASSERT_EQUALS(ull, 123456789101112ULL);
    TS_ASSERT(strToInteger("123456789101112", ll));
    TS_ASSERT_EQUALS(ll, 123456789101112LL);

    TS_ASSERT(!strToInteger("-123456789101112", u64));
    TS_ASSERT(strToInteger("-123456789101112", i64));
    TS_ASSERT_EQUALS(i64, -123456789101112LL);
    TS_ASSERT(!strToInteger("-123456789101112", ull));
    TS_ASSERT(strToInteger("-123456789101112", ll));
    TS_ASSERT_EQUALS(ll, -123456789101112LL);

    TS_ASSERT(!strToInteger("99999999999999999999999999999999", u64));
    TS_ASSERT(!strToInteger("99999999999999999999999999999999", i64));

    // Border cases
    TS_ASSERT(!strToInteger("", ll));
    TS_ASSERT(!strToInteger(" ", ll));
}

/** Test strToFloat. */
void
TestStringParse::testStrToFloat()
{
    using afl::string::strToFloat;

    double d;
    TS_ASSERT(strToFloat("1", d));
    TS_ASSERT_EQUALS(d, 1.0);

    TS_ASSERT(strToFloat("99", d));
    TS_ASSERT_EQUALS(d, 99.0);

    TS_ASSERT(strToFloat("-1", d));
    TS_ASSERT_EQUALS(d, -1.0);

    TS_ASSERT(strToFloat("-1.5", d));
    TS_ASSERT_EQUALS(d, -1.5);

    TS_ASSERT(strToFloat("2.5e10", d));
    TS_ASSERT_EQUALS(d, 2.5e10);

    TS_ASSERT(!strToFloat("", d));
    TS_ASSERT(!strToFloat("x", d));
    // TS_ASSERT(!strToFloat("0x123", d)); // this is actually considered valid by some
    TS_ASSERT(!strToFloat("0k123", d));
    TS_ASSERT(!strToFloat("12.23.34", d));

    String_t::size_type pos;
    // TS_ASSERT(!strToFloat("0x123", d, pos));
    TS_ASSERT(!strToFloat("0k123", d, pos));
    TS_ASSERT_EQUALS(pos, 1U);
    TS_ASSERT(!strToFloat("12.23.34", d, pos));
    TS_ASSERT_EQUALS(pos, 5U);
}
