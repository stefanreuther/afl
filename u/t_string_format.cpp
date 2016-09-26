/**
  *  \file u/t_string_format.cpp
  *  \brief Test for afl::string::Format
  */

#include "afl/string/format.hpp"

#include "u/t_string.hpp"

/** Test conversion/construction. */
void
TestStringFormat::testConv()
{
    using afl::string::Format;

    // This mainly tests correct initialisation/conversion
    {
        String_t s = Format("foo%d") << 42;
        TS_ASSERT_EQUALS(s, "foo42");
    }
    {
        String_t s = Format("foo%d", 42);
        TS_ASSERT_EQUALS(s, "foo42");
    }

    // We have up to 4 args in the constructor
    {
        String_t s = Format("%d-%d", 1, 2U);
        TS_ASSERT_EQUALS(s, "1-2");
    }
    {
        String_t s = Format("%d-%d-%d", 1, 2U, "3");
        TS_ASSERT_EQUALS(s, "1-2-3");
    }
    {
        String_t s = Format("%d-%d-%d-%d", 1, 2U, "3", '\4');
        TS_ASSERT_EQUALS(s, "1-2-3-4");
    }
}

/** Test formatting of integer types. */
void
TestStringFormat::testTypes()
{
    using afl::string::Format;

    // Base types
    TS_ASSERT_EQUALS(String_t(Format("%d", (char)           42)), "42");
    TS_ASSERT_EQUALS(String_t(Format("%d", (signed char)    42)), "42");
    TS_ASSERT_EQUALS(String_t(Format("%d", (unsigned char)  42)), "42");
    TS_ASSERT_EQUALS(String_t(Format("%d", (short)          42)), "42");
    TS_ASSERT_EQUALS(String_t(Format("%d", (unsigned short) 42)), "42");
    TS_ASSERT_EQUALS(String_t(Format("%d", (int)            42)), "42");
    TS_ASSERT_EQUALS(String_t(Format("%d", (unsigned int)   42)), "42");
    TS_ASSERT_EQUALS(String_t(Format("%d", (long)           42)), "42");
    TS_ASSERT_EQUALS(String_t(Format("%d", (unsigned long)  42)), "42");

    TS_ASSERT_EQUALS(String_t(Format("%d", (signed char)   -42)), "-42");
    TS_ASSERT_EQUALS(String_t(Format("%d", (short)         -42)), "-42");
    TS_ASSERT_EQUALS(String_t(Format("%d", (int)           -42)), "-42");
    TS_ASSERT_EQUALS(String_t(Format("%d", (long)          -42)), "-42");

    // Bit widths
    TS_ASSERT_EQUALS(String_t(Format("%d", (uint8_t)  42)), "42");
    TS_ASSERT_EQUALS(String_t(Format("%d", (uint16_t) 42)), "42");
    TS_ASSERT_EQUALS(String_t(Format("%d", (uint32_t) 42)), "42");
    TS_ASSERT_EQUALS(String_t(Format("%d", (uint64_t) 42)), "42");

    TS_ASSERT_EQUALS(String_t(Format("%d", (int8_t)   42)), "42");
    TS_ASSERT_EQUALS(String_t(Format("%d", (int16_t)  42)), "42");
    TS_ASSERT_EQUALS(String_t(Format("%d", (int32_t)  42)), "42");
    TS_ASSERT_EQUALS(String_t(Format("%d", (int64_t)  42)), "42");

    TS_ASSERT_EQUALS(String_t(Format("%d", (int8_t)  -42)), "-42");
    TS_ASSERT_EQUALS(String_t(Format("%d", (int16_t) -42)), "-42");
    TS_ASSERT_EQUALS(String_t(Format("%d", (int32_t) -42)), "-42");
    TS_ASSERT_EQUALS(String_t(Format("%d", (int64_t) -42)), "-42");

    // Specials
    TS_ASSERT_EQUALS(String_t(Format("%d", (size_t)    42)), "42");
    TS_ASSERT_EQUALS(String_t(Format("%d", (uintptr_t) 42)), "42");
    TS_ASSERT_EQUALS(String_t(Format("%d", (uintmax_t) 42)), "42");

    TS_ASSERT_EQUALS(String_t(Format("%d", (ptrdiff_t) 42)), "42");
    TS_ASSERT_EQUALS(String_t(Format("%d", (intptr_t)  42)), "42");
    TS_ASSERT_EQUALS(String_t(Format("%d", (intmax_t)  42)), "42");

    TS_ASSERT_EQUALS(String_t(Format("%d", (ptrdiff_t) -42)), "-42");
    TS_ASSERT_EQUALS(String_t(Format("%d", (intptr_t)  -42)), "-42");
    TS_ASSERT_EQUALS(String_t(Format("%d", (intmax_t)  -42)), "-42");
}

/** Test integer formats. */
void
TestStringFormat::testInt()
{
    using afl::string::Format;

    TS_ASSERT_EQUALS(String_t(Format("dezimal            = `%d'") << 42),        "dezimal            = `42'");
    TS_ASSERT_EQUALS(String_t(Format("hex                = `%x'") << 42),        "hex                = `2a'");
    TS_ASSERT_EQUALS(String_t(Format("oct                = `%o'") << 42),        "oct                = `52'");
    TS_ASSERT_EQUALS(String_t(Format("alt.hex            = `%#x'") << 42),       "alt.hex            = `0x2a'");
    TS_ASSERT_EQUALS(String_t(Format("alt.oct            = `%#o'") << 42),       "alt.oct            = `052'");
    TS_ASSERT_EQUALS(String_t(Format("alt.dezimal        = `%#d'") << 42),       "alt.dezimal        = `42'");
    TS_ASSERT_EQUALS(String_t(Format("group dez          = `%'d'") << 42),       "group dez          = `42'");
    TS_ASSERT_EQUALS(String_t(Format("group oct          = `%'o'") << 42),       "group oct          = `52'");
    TS_ASSERT_EQUALS(String_t(Format("10-dez             = `%10d'") << 42),      "10-dez             = `        42'");
    TS_ASSERT_EQUALS(String_t(Format("left 10-dez        = `%-10d'") << 42),     "left 10-dez        = `42        '");
    TS_ASSERT_EQUALS(String_t(Format("left group 10-dez  = `%'-10d'") << 42),    "left group 10-dez  = `42        '");
    TS_ASSERT_EQUALS(String_t(Format("group 10-dez       = `%'10d'") << 42),     "group 10-dez       = `        42'");
    TS_ASSERT_EQUALS(String_t(Format("zpad 10-dez        = `%010d'") << 42),     "zpad 10-dez        = `0000000042'");
    TS_ASSERT_EQUALS(String_t(Format("sign zpad 10-dez   = `%+010d'") << 42),    "sign zpad 10-dez   = `+000000042'");
    TS_ASSERT_EQUALS(String_t(Format("group 10-dez       = `%'10d'") << 1024),   "group 10-dez       = `     1,024'");
    TS_ASSERT_EQUALS(String_t(Format("group 10-dez       = `%'10d'") << 45054),  "group 10-dez       = `    45,054'");
    TS_ASSERT_EQUALS(String_t(Format("group 10-hex       = `%'10X'") << 45054),  "group 10-hex       = `     A,FFE'");
    TS_ASSERT_EQUALS(String_t(Format("group alt 10-hex   = `%#'10X'") << 45054), "group alt 10-hex   = `   0XA,FFE'");
    TS_ASSERT_EQUALS(String_t(Format("group alt 10 dez   = `%#'10X'") << 999),   "group alt 10 dez   = `     0X3E7'");
    TS_ASSERT_EQUALS(String_t(Format("group alt neg dez  = `%#'d'") << -999),    "group alt neg dez  = `-999'");
    TS_ASSERT_EQUALS(String_t(Format("group alt neg10dez = `%#'10d'") << -999),  "group alt neg10dez = `      -999'");
    TS_ASSERT_EQUALS(String_t(Format("zpad alt neg10dez  = `%#010d'") << -999),  "zpad alt neg10dez  = `-000000999'");
    TS_ASSERT_EQUALS(String_t(Format("neg 10 dez         = `%#10d'") << -4711),  "neg 10 dez         = `     -4711'");
    TS_ASSERT_EQUALS(String_t(Format("left neg 10 dez    = `%-#10d'") << -4711), "left neg 10 dez    = `-4711     '");
    TS_ASSERT_EQUALS(String_t(Format("grp neg dez        = `%'d'") << -4711),    "grp neg dez        = `-4,711'");

    static const char* str = "cond = `%d %1{item for %d dollar%!1{s%}%|items for %d dollar%!1{s%} each%}'";

    TS_ASSERT_EQUALS(String_t(Format(str) << 0 << 0), "cond = `0 items for 0 dollars each'");
    TS_ASSERT_EQUALS(String_t(Format(str) << 0 << 1), "cond = `0 items for 1 dollar each'");
    TS_ASSERT_EQUALS(String_t(Format(str) << 0 << 2), "cond = `0 items for 2 dollars each'");
    TS_ASSERT_EQUALS(String_t(Format(str) << 1 << 0), "cond = `1 item for 0 dollars'");
    TS_ASSERT_EQUALS(String_t(Format(str) << 1 << 1), "cond = `1 item for 1 dollar'");
    TS_ASSERT_EQUALS(String_t(Format(str) << 1 << 2), "cond = `1 item for 2 dollars'");
    TS_ASSERT_EQUALS(String_t(Format(str) << 2 << 0), "cond = `2 items for 0 dollars each'");
    TS_ASSERT_EQUALS(String_t(Format(str) << 2 << 1), "cond = `2 items for 1 dollar each'");
    TS_ASSERT_EQUALS(String_t(Format(str) << 2 << 2), "cond = `2 items for 2 dollars each'");
}

/** Test unsigned formats. */
void
TestStringFormat::testUInt()
{
    using afl::string::Format;

    TS_ASSERT_EQUALS(String_t(Format("dezimal            = `%d'") << 42U),        "dezimal            = `42'");
    TS_ASSERT_EQUALS(String_t(Format("hex                = `%x'") << 42U),        "hex                = `2a'");
    TS_ASSERT_EQUALS(String_t(Format("oct                = `%o'") << 42U),        "oct                = `52'");
    TS_ASSERT_EQUALS(String_t(Format("alt.hex            = `%#x'") << 42U),       "alt.hex            = `0x2a'");
    TS_ASSERT_EQUALS(String_t(Format("alt.oct            = `%#o'") << 42U),       "alt.oct            = `052'");
    TS_ASSERT_EQUALS(String_t(Format("alt.dezimal        = `%#d'") << 42U),       "alt.dezimal        = `42'");
    TS_ASSERT_EQUALS(String_t(Format("group dez          = `%'d'") << 42U),       "group dez          = `42'");
    TS_ASSERT_EQUALS(String_t(Format("group oct          = `%'o'") << 42U),       "group oct          = `52'");
    TS_ASSERT_EQUALS(String_t(Format("10-dez             = `%10d'") << 42U),      "10-dez             = `        42'");
    TS_ASSERT_EQUALS(String_t(Format("left 10-dez        = `%-10d'") << 42U),     "left 10-dez        = `42        '");
    TS_ASSERT_EQUALS(String_t(Format("left group 10-dez  = `%'-10d'") << 42U),    "left group 10-dez  = `42        '");
    TS_ASSERT_EQUALS(String_t(Format("group 10-dez       = `%'10d'") << 42U),     "group 10-dez       = `        42'");
    TS_ASSERT_EQUALS(String_t(Format("zpad 10-dez        = `%010d'") << 42U),     "zpad 10-dez        = `0000000042'");
    TS_ASSERT_EQUALS(String_t(Format("sign zpad 10-dez   = `%+010d'") << 42U),    "sign zpad 10-dez   = `+000000042'");
    TS_ASSERT_EQUALS(String_t(Format("group 10-dez       = `%'10d'") << 1024U),   "group 10-dez       = `     1,024'");
    TS_ASSERT_EQUALS(String_t(Format("group 10-dez       = `%'10d'") << 45054U),  "group 10-dez       = `    45,054'");
    TS_ASSERT_EQUALS(String_t(Format("group 10-hex       = `%'10X'") << 45054U),  "group 10-hex       = `     A,FFE'");
    TS_ASSERT_EQUALS(String_t(Format("group alt 10-hex   = `%#'10X'") << 45054U), "group alt 10-hex   = `   0XA,FFE'");
    TS_ASSERT_EQUALS(String_t(Format("group alt 10 dez   = `%#'10X'") << 999U),   "group alt 10 dez   = `     0X3E7'");

    static const char* str = "cond = `%d %1{item for %d dollar%!1{s%}%|items for %d dollar%!1{s%} each%}'";

    TS_ASSERT_EQUALS(String_t(Format(str) << 0U << 0U), "cond = `0 items for 0 dollars each'");
    TS_ASSERT_EQUALS(String_t(Format(str) << 0U << 1U), "cond = `0 items for 1 dollar each'");
    TS_ASSERT_EQUALS(String_t(Format(str) << 0U << 2U), "cond = `0 items for 2 dollars each'");
    TS_ASSERT_EQUALS(String_t(Format(str) << 1U << 0U), "cond = `1 item for 0 dollars'");
    TS_ASSERT_EQUALS(String_t(Format(str) << 1U << 1U), "cond = `1 item for 1 dollar'");
    TS_ASSERT_EQUALS(String_t(Format(str) << 1U << 2U), "cond = `1 item for 2 dollars'");
    TS_ASSERT_EQUALS(String_t(Format(str) << 2U << 0U), "cond = `2 items for 0 dollars each'");
    TS_ASSERT_EQUALS(String_t(Format(str) << 2U << 1U), "cond = `2 items for 1 dollar each'");
    TS_ASSERT_EQUALS(String_t(Format(str) << 2U << 2U), "cond = `2 items for 2 dollars each'");
}

/** Test floating-point formats. */
void
TestStringFormat::testFP()
{
    using afl::string::Format;

    /* floating-point tests */
    TS_ASSERT_EQUALS(String_t(Format("Format e           = `%e'") << 4711.0),     "Format e           = `4.711000e+03'");
    TS_ASSERT_EQUALS(String_t(Format("Format f           = `%f'") << 4711.0),     "Format f           = `4711.000000'");
    TS_ASSERT_EQUALS(String_t(Format("Format g           = `%g'") << 4711.0),     "Format g           = `4711'");
    TS_ASSERT_EQUALS(String_t(Format("Format e one       = `%e'") << 1.0),        "Format e one       = `1.000000e+00'");
    TS_ASSERT_EQUALS(String_t(Format("Format f one       = `%f'") << 1.0),        "Format f one       = `1.000000'");
    TS_ASSERT_EQUALS(String_t(Format("Format g one       = `%g'") << 1.0),        "Format g one       = `1'");
    TS_ASSERT_EQUALS(String_t(Format("Format e zero      = `%e'") << 0.0),        "Format e zero      = `0.000000e+00'");
    TS_ASSERT_EQUALS(String_t(Format("Format f zero      = `%f'") << 0.0),        "Format f zero      = `0.000000'");
    TS_ASSERT_EQUALS(String_t(Format("Format g zero      = `%g'") << 0.0),        "Format g zero      = `0'");
    TS_ASSERT_EQUALS(String_t(Format("Format e tiny      = `%e'") << 0.00000123), "Format e tiny      = `1.230000e-06'");
    TS_ASSERT_EQUALS(String_t(Format("Format f tiny      = `%f'") << 0.00000123), "Format f tiny      = `0.000001'");
    TS_ASSERT_EQUALS(String_t(Format("Format g tiny      = `%g'") << 0.00000123), "Format g tiny      = `1.23e-06'");
    TS_ASSERT_EQUALS(String_t(Format("Format .2g         = `%.2g'") << 12345.0),  "Format .2g         = `1.23e+04'");

    /* more floating-point tests */
    TS_ASSERT_EQUALS(String_t(Format("Format .2f .999     = `%.2f'") << 0.999),   "Format .2f .999     = `1.00'");
    TS_ASSERT_EQUALS(String_t(Format("Format .2e .999     = `%.2e'") << 0.999),   "Format .2e .999     = `9.99e-01'");
    TS_ASSERT_EQUALS(String_t(Format("Format .2f 99.999   = `%.2f'") << 99.999),  "Format .2f 99.999   = `100.00'");
    TS_ASSERT_EQUALS(String_t(Format("Format .2e 99.999   = `%.2e'") << 99.999),  "Format .2e 99.999   = `1.00e+02'");
    TS_ASSERT_EQUALS(String_t(Format("Format .5f 99.999   = `%.5f'") << 99.999),  "Format .5f 99.999   = `99.99900'");
    TS_ASSERT_EQUALS(String_t(Format("Format .5e 99.999   = `%.5e'") << 99.999),  "Format .5e 99.999   = `9.99990e+01'");
    TS_ASSERT_EQUALS(String_t(Format("Format .2f 25       = `%.2f'") << 25.0),    "Format .2f 25       = `25.00'");
    TS_ASSERT_EQUALS(String_t(Format("Format .2e 25       = `%.2e'") << 25.0),    "Format .2e 25       = `2.50e+01'");
    TS_ASSERT_EQUALS(String_t(Format("Format .2f .25      = `%.2f'") << 0.25),    "Format .2f .25      = `0.25'");
    TS_ASSERT_EQUALS(String_t(Format("Format .2e .25      = `%.2e'") << 0.25),    "Format .2e .25      = `2.50e-01'");

    TS_ASSERT_EQUALS(String_t(Format("Format g hundred    = `%g'") << 100.0),     "Format g hundred    = `100'");
    TS_ASSERT_EQUALS(String_t(Format("Format f hundred    = `%.0f'") << 100.0),   "Format f hundred    = `100'");
    TS_ASSERT_EQUALS(String_t(Format("Format f hundred    = `%.1f'") << 100.0),   "Format f hundred    = `100.0'");

    TS_ASSERT_EQUALS(String_t(Format("%.1f", 0.003003003)), "0.0");
    TS_ASSERT_EQUALS(String_t(Format("%.1f", 0.99)), "1.0");
    TS_ASSERT_EQUALS(String_t(Format("%.1f", 99.99)), "100.0");
}

/** Test that strings formatted as integers set flags. */
void
TestStringFormat::testIntStr()
{
    using afl::string::Format;

    static const char* str = "cond = `%d %1{item for %d dollar%!1{s%}%|items for %d dollar%!1{s%} each%}'";

    // Using const char*:
    TS_ASSERT_EQUALS(String_t(Format(str) << "0" << "0"), "cond = `0 items for 0 dollars each'");
    TS_ASSERT_EQUALS(String_t(Format(str) << "0" << "1"), "cond = `0 items for 1 dollar each'");
    TS_ASSERT_EQUALS(String_t(Format(str) << "0" << "2"), "cond = `0 items for 2 dollars each'");
    TS_ASSERT_EQUALS(String_t(Format(str) << "1" << "0"), "cond = `1 item for 0 dollars'");
    TS_ASSERT_EQUALS(String_t(Format(str) << "1" << "1"), "cond = `1 item for 1 dollar'");
    TS_ASSERT_EQUALS(String_t(Format(str) << "1" << "2"), "cond = `1 item for 2 dollars'");
    TS_ASSERT_EQUALS(String_t(Format(str) << "2" << "0"), "cond = `2 items for 0 dollars each'");
    TS_ASSERT_EQUALS(String_t(Format(str) << "2" << "1"), "cond = `2 items for 1 dollar each'");
    TS_ASSERT_EQUALS(String_t(Format(str) << "2" << "2"), "cond = `2 items for 2 dollars each'");

    TS_ASSERT_EQUALS(String_t(Format(str) << "0000000000000" << "0000000000000"), "cond = `0000000000000 items for 0000000000000 dollars each'");
    TS_ASSERT_EQUALS(String_t(Format(str) << "0000000000000" << "0000000000001"), "cond = `0000000000000 items for 0000000000001 dollar each'");
    TS_ASSERT_EQUALS(String_t(Format(str) << "0000000000000" << "0000000000002"), "cond = `0000000000000 items for 0000000000002 dollars each'");
    TS_ASSERT_EQUALS(String_t(Format(str) << "000000000001x" << "0004294967297"), "cond = `000000000001x item for 0004294967297 dollars'");

    TS_ASSERT_EQUALS(String_t(Format(str) << "000,000,000,1" << ",,,,,,0,,0,,,"), "cond = `000,000,000,1 item for ,,,,,,0,,0,,, dollars'");
    
    // Using String_t:
    TS_ASSERT_EQUALS(String_t(Format(str) << String_t("0") << String_t("0")), "cond = `0 items for 0 dollars each'");
    TS_ASSERT_EQUALS(String_t(Format(str) << String_t("0") << String_t("1")), "cond = `0 items for 1 dollar each'");
    TS_ASSERT_EQUALS(String_t(Format(str) << String_t("0") << String_t("2")), "cond = `0 items for 2 dollars each'");
    TS_ASSERT_EQUALS(String_t(Format(str) << String_t("1") << String_t("0")), "cond = `1 item for 0 dollars'");
    TS_ASSERT_EQUALS(String_t(Format(str) << String_t("1") << String_t("1")), "cond = `1 item for 1 dollar'");
    TS_ASSERT_EQUALS(String_t(Format(str) << String_t("1") << String_t("2")), "cond = `1 item for 2 dollars'");
    TS_ASSERT_EQUALS(String_t(Format(str) << String_t("2") << String_t("0")), "cond = `2 items for 0 dollars each'");
    TS_ASSERT_EQUALS(String_t(Format(str) << String_t("2") << String_t("1")), "cond = `2 items for 1 dollar each'");
    TS_ASSERT_EQUALS(String_t(Format(str) << String_t("2") << String_t("2")), "cond = `2 items for 2 dollars each'");

    TS_ASSERT_EQUALS(String_t(Format(str) << String_t("0000000000000") << String_t("0000000000000")), "cond = `0000000000000 items for 0000000000000 dollars each'");
    TS_ASSERT_EQUALS(String_t(Format(str) << String_t("0000000000000") << String_t("0000000000001")), "cond = `0000000000000 items for 0000000000001 dollar each'");
    TS_ASSERT_EQUALS(String_t(Format(str) << String_t("0000000000000") << String_t("0000000000002")), "cond = `0000000000000 items for 0000000000002 dollars each'");
    TS_ASSERT_EQUALS(String_t(Format(str) << String_t("000000000001x") << String_t("0004294967297")), "cond = `000000000001x item for 0004294967297 dollars'");

    String_t one = "11";
    TS_ASSERT_EQUALS(String_t(Format(str) << one << one), "cond = `11 items for 11 dollars each'");
    one.erase(1);
    TS_ASSERT_EQUALS(String_t(Format(str) << one << one), "cond = `1 item for 1 dollar'");
}

