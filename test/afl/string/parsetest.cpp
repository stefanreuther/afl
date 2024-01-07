/**
  *  \file test/afl/string/parsetest.cpp
  *  \brief Test for afl::string::Parse
  */

#include "afl/string/parse.hpp"
#include "afl/test/testrunner.hpp"

/** Test strToInteger. */
AFL_TEST("afl.string.Parse:strToInteger", as)
{
    using afl::string::strToInteger;

    String_t a = "126";
    String_t b = "42km";
    String_t c = "45 ";
    String_t d = " 17 ";
    String_t e = "   99  miles ";
    int res = -1;
    String_t::size_type pos;

    as.check("01. strToInteger", strToInteger(a, res));
    as.checkEqual("02. result", res, 126);

    as.check("11. strToInteger", !strToInteger(b, res));
    as.check("12. strToInteger", !strToInteger(b, res, pos));
    as.checkEqual("13. pos", pos, 2U);
    as.check("14. strToInteger", strToInteger(b.substr(0, pos), res));
    as.checkEqual("15. res", res, 42);

    as.check("21. strToInteger", strToInteger(c, res));
    as.checkEqual("22. res", res, 45);

    as.check("31. strToInteger", strToInteger(d, res));
    as.checkEqual("32. res", res, 17);

    as.check("41. strToInteger", !strToInteger(e, res));
    as.check("42. strToInteger", !strToInteger(e, res, pos));
    as.checkEqual("43. pos", pos, 7U);
    as.check("44. strToInteger", strToInteger(e.substr(0, pos), res));
    as.checkEqual("45. res", res, 99);

    // Border cases
    as.check("51. strToInteger", !strToInteger("", res));
    as.check("52. strToInteger", !strToInteger(" ", res));

    // Now with overflow checking
    String_t f = "240";
    int8_t i8 = 0;
    uint8_t u8 = 0;
    long il = 0;
    unsigned long ul = 0;
    as.check("61. strToInteger", !strToInteger(f, i8));
    as.check("62. strToInteger", strToInteger(f, u8));
    as.checkEqual("63. res", u8, 240U);

    String_t t = "1000";
    as.check("71. strToInteger", !strToInteger(t, i8));
    as.check("72. strToInteger", !strToInteger(t, u8));

    // Negative
    String_t g = "-10";
    as.check("81. strToInteger", strToInteger(g, i8));
    as.check("82. strToInteger", !strToInteger(g, u8));
    as.checkEqual("83. res", i8, -10);

    as.check("91. strToInteger", strToInteger(g, il));
    as.check("92. strToInteger", !strToInteger(g, ul));
    as.checkEqual("93. res", il, -10);
}

/** Test conversion to long long. */
AFL_TEST("afl.string.Parse:strToInteger:longlong", as)
{
    using afl::string::strToInteger;

    uint64_t u64;
    int64_t i64;
    unsigned long long ull;
    long long ll;

    as.check("01. strToInteger", strToInteger("1", u64));
    as.checkEqual("02. res", u64, 1U);
    as.check("03. strToInteger", strToInteger("1", i64));
    as.checkEqual("04. res", i64, 1);
    as.check("05. strToInteger", strToInteger("1", ull));
    as.checkEqual("06. res", ull, 1U);
    as.check("07. strToInteger", strToInteger("1", ll));
    as.checkEqual("08. res", ll, 1);

    as.check("11. strToInteger", strToInteger("123456789101112", u64));
    as.checkEqual("12. res", u64, 123456789101112ULL);
    as.check("13. strToInteger", strToInteger("123456789101112", i64));
    as.checkEqual("14. res", i64, 123456789101112LL);
    as.check("15. strToInteger", strToInteger("123456789101112", ull));
    as.checkEqual("16. res", ull, 123456789101112ULL);
    as.check("17. strToInteger", strToInteger("123456789101112", ll));
    as.checkEqual("18. res", ll, 123456789101112LL);

    as.check("21. strToInteger", !strToInteger("-123456789101112", u64));
    as.check("22. strToInteger", strToInteger("-123456789101112", i64));
    as.checkEqual("23. res", i64, -123456789101112LL);
    as.check("24. strToInteger", !strToInteger("-123456789101112", ull));
    as.check("25. strToInteger", strToInteger("-123456789101112", ll));
    as.checkEqual("26. res", ll, -123456789101112LL);

    as.check("31. strToInteger", !strToInteger("99999999999999999999999999999999", u64));
    as.check("32. strToInteger", !strToInteger("99999999999999999999999999999999", i64));

    // Border cases
    as.check("41. strToInteger", !strToInteger("", ll));
    as.check("42. strToInteger", !strToInteger(" ", ll));
}

/** Test strToFloat. */
AFL_TEST("afl.string.Parse:strToFloat", as)
{
    using afl::string::strToFloat;

    double d;
    as.check("01. strToFloat", strToFloat("1", d));
    as.checkEqual("02. res", d, 1.0);

    as.check("03. strToFloat", strToFloat("99", d));
    as.checkEqual("04. res", d, 99.0);

    as.check("05. strToFloat", strToFloat("-1", d));
    as.checkEqual("06. res", d, -1.0);

    as.check("07. strToFloat", strToFloat("-1.5", d));
    as.checkEqual("08. res", d, -1.5);

    as.check("09. strToFloat", strToFloat("2.5e10", d));
    as.checkEqual("10. res", d, 2.5e10);

    as.check("21. strToFloat", !strToFloat("", d));
    as.check("22. strToFloat", !strToFloat("x", d));
    // as.check("", !strToFloat("0x123", d)); // this is actually considered valid by some
    as.check("24. strToFloat", !strToFloat("0k123", d));
    as.check("25. strToFloat", !strToFloat("12.23.34", d));

    String_t::size_type pos;
    // as.check("", !strToFloat("0x123", d, pos));
    as.check("32. strToFloat", !strToFloat("0k123", d, pos));
    as.checkEqual("33. pos", pos, 1U);
    as.check("34. strToFloat", !strToFloat("12.23.34", d, pos));
    as.checkEqual("35. pos", pos, 5U);
}
