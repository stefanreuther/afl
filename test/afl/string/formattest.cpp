/**
  *  \file test/afl/string/formattest.cpp
  *  \brief Test for afl::string::Format
  */

#include "afl/string/format.hpp"

#include <sstream>
#include <cmath>
#include "afl/test/testrunner.hpp"

using afl::string::Format;

/*
 *  Conversion/construction
 */

// This mainly tests correct initialisation/conversion
AFL_TEST("afl.string.Format:init:charp-shift-int", a)
{
    String_t s = Format("foo%d") << 42;
    a.checkEqual("", s, "foo42");
}

AFL_TEST("afl.string.Format:init:charp-int", a)
{
    String_t s = Format("foo%d", 42);
    a.checkEqual("", s, "foo42");
}

AFL_TEST("afl.string.Format:init:string-shift-int", a)
{
    String_t s = Format(String_t("foo%d")) << 42;
    a.checkEqual("", s, "foo42");
}

AFL_TEST("afl.string.Format:init:string-int", a)
{
    String_t s = Format(String_t("foo%d"), 42);
    a.checkEqual("", s, "foo42");
}

// We have up to 4 args in the constructor
AFL_TEST("afl.string.Format:init:charp-2args", a)
{
    String_t s = Format("%d-%d", 1, 2U);
    a.checkEqual("", s, "1-2");
}

AFL_TEST("afl.string.Format:init:charp-3args", a)
{
    String_t s = Format("%d-%d-%d", 1, 2U, "3");
    a.checkEqual("", s, "1-2-3");
}

AFL_TEST("afl.string.Format:init:charp-4args", a)
{
    String_t s = Format("%d-%d-%d-%d", 1, 2U, "3", '\4');
    a.checkEqual("", s, "1-2-3-4");
}

AFL_TEST("afl.string.Format:init:string-2args", a)
{
    String_t s = Format(String_t("%d-%d"), 1, 2U);
    a.checkEqual("", s, "1-2");
}

AFL_TEST("afl.string.Format:init:string-3args", a)
{
    String_t s = Format(String_t("%d-%d-%d"), 1, 2U, "3");
    a.checkEqual("", s, "1-2-3");
}

AFL_TEST("afl.string.Format:init:string-4args", a)
{
    String_t s = Format(String_t("%d-%d-%d-%d"), 1, 2U, "3", '\4');
    a.checkEqual("", s, "1-2-3-4");
}

/** Test formatting of integer types. */
AFL_TEST("afl.string.Format:int-types", a)
{
    // Base types
    a.checkEqual("char",      String_t(Format("%d", (char)           42)), "42");
    a.checkEqual("schar",     String_t(Format("%d", (signed char)    42)), "42");
    a.checkEqual("uchar",     String_t(Format("%d", (unsigned char)  42)), "42");
    a.checkEqual("short",     String_t(Format("%d", (short)          42)), "42");
    a.checkEqual("ushort",    String_t(Format("%d", (unsigned short) 42)), "42");
    a.checkEqual("int",       String_t(Format("%d", (int)            42)), "42");
    a.checkEqual("uint",      String_t(Format("%d", (unsigned int)   42)), "42");
    a.checkEqual("long",      String_t(Format("%d", (long)           42)), "42");
    a.checkEqual("ulong",     String_t(Format("%d", (unsigned long)  42)), "42");

    a.checkEqual("schar-neg", String_t(Format("%d", (signed char)   -42)), "-42");
    a.checkEqual("short-neg", String_t(Format("%d", (short)         -42)), "-42");
    a.checkEqual("int-neg",   String_t(Format("%d", (int)           -42)), "-42");
    a.checkEqual("long-neg",  String_t(Format("%d", (long)          -42)), "-42");

    // Bit widths
    a.checkEqual("u8",        String_t(Format("%d", (uint8_t)  42)), "42");
    a.checkEqual("u16",       String_t(Format("%d", (uint16_t) 42)), "42");
    a.checkEqual("u32",       String_t(Format("%d", (uint32_t) 42)), "42");
    a.checkEqual("u64",       String_t(Format("%d", (uint64_t) 42)), "42");

    a.checkEqual("s8",        String_t(Format("%d", (int8_t)   42)), "42");
    a.checkEqual("s16",       String_t(Format("%d", (int16_t)  42)), "42");
    a.checkEqual("s32",       String_t(Format("%d", (int32_t)  42)), "42");
    a.checkEqual("s64",       String_t(Format("%d", (int64_t)  42)), "42");

    a.checkEqual("i8-neg",    String_t(Format("%d", (int8_t)  -42)), "-42");
    a.checkEqual("i16-neg",   String_t(Format("%d", (int16_t) -42)), "-42");
    a.checkEqual("i32-neg",   String_t(Format("%d", (int32_t) -42)), "-42");
    a.checkEqual("i64-neg",   String_t(Format("%d", (int64_t) -42)), "-42");

    // Specials
    a.checkEqual("size",      String_t(Format("%d", (size_t)    42)), "42");
    a.checkEqual("uptr",      String_t(Format("%d", (uintptr_t) 42)), "42");
    a.checkEqual("umax",      String_t(Format("%d", (uintmax_t) 42)), "42");

    a.checkEqual("ptrd",      String_t(Format("%d", (ptrdiff_t) 42)), "42");
    a.checkEqual("iptr",      String_t(Format("%d", (intptr_t)  42)), "42");
    a.checkEqual("imax",      String_t(Format("%d", (intmax_t)  42)), "42");

    a.checkEqual("ptrd-neg",  String_t(Format("%d", (ptrdiff_t) -42)), "-42");
    a.checkEqual("iptr-neg",  String_t(Format("%d", (intptr_t)  -42)), "-42");
    a.checkEqual("imax-neg",  String_t(Format("%d", (intmax_t)  -42)), "-42");
}

/** Test integer formats. */
AFL_TEST("afl.string.Format:int-formats", a)
{
    a.checkEqual("%d",      String_t(Format("dezimal            = `%d'") << 42),        "dezimal            = `42'");
    a.checkEqual("%x",      String_t(Format("hex                = `%x'") << 42),        "hex                = `2a'");
    a.checkEqual("%o",      String_t(Format("oct                = `%o'") << 42),        "oct                = `52'");
    a.checkEqual("%#x",     String_t(Format("alt.hex            = `%#x'") << 42),       "alt.hex            = `0x2a'");
    a.checkEqual("%#o",     String_t(Format("alt.oct            = `%#o'") << 42),       "alt.oct            = `052'");
    a.checkEqual("%#x",     String_t(Format("alt.dezimal        = `%#d'") << 42),       "alt.dezimal        = `42'");
    a.checkEqual("%'d",     String_t(Format("group dez          = `%'d'") << 42),       "group dez          = `42'");
    a.checkEqual("%'o",     String_t(Format("group oct          = `%'o'") << 42),       "group oct          = `52'");
    a.checkEqual("%10d",    String_t(Format("10-dez             = `%10d'") << 42),      "10-dez             = `        42'");
    a.checkEqual("%-10d",   String_t(Format("left 10-dez        = `%-10d'") << 42),     "left 10-dez        = `42        '");
    a.checkEqual("%'-10d",  String_t(Format("left group 10-dez  = `%'-10d'") << 42),    "left group 10-dez  = `42        '");
    a.checkEqual("%'10d",   String_t(Format("group 10-dez       = `%'10d'") << 42),     "group 10-dez       = `        42'");
    a.checkEqual("%010d",   String_t(Format("zpad 10-dez        = `%010d'") << 42),     "zpad 10-dez        = `0000000042'");
    a.checkEqual("%+010d",  String_t(Format("sign zpad 10-dez   = `%+010d'") << 42),    "sign zpad 10-dez   = `+000000042'");
    a.checkEqual("%'10d",   String_t(Format("group 10-dez       = `%'10d'") << 1024),   "group 10-dez       = `     1,024'");
    a.checkEqual("%'10d 2", String_t(Format("group 10-dez       = `%'10d'") << 45054),  "group 10-dez       = `    45,054'");
    a.checkEqual("%'10X",   String_t(Format("group 10-hex       = `%'10X'") << 45054),  "group 10-hex       = `     A,FFE'");
    a.checkEqual("%#'10X",  String_t(Format("group alt 10-hex   = `%#'10X'") << 45054), "group alt 10-hex   = `   0XA,FFE'");
    a.checkEqual("%#'10X",  String_t(Format("group alt 10 dez   = `%#'10X'") << 999),   "group alt 10 dez   = `     0X3E7'");
    a.checkEqual("%#'d",    String_t(Format("group alt neg dez  = `%#'d'") << -999),    "group alt neg dez  = `-999'");
    a.checkEqual("%#'10d",  String_t(Format("group alt neg10dez = `%#'10d'") << -999),  "group alt neg10dez = `      -999'");
    a.checkEqual("%#010d",  String_t(Format("zpad alt neg10dez  = `%#010d'") << -999),  "zpad alt neg10dez  = `-000000999'");
    a.checkEqual("%#10d",   String_t(Format("neg 10 dez         = `%#10d'") << -4711),  "neg 10 dez         = `     -4711'");
    a.checkEqual("%-#10d",  String_t(Format("left neg 10 dez    = `%-#10d'") << -4711), "left neg 10 dez    = `-4711     '");
    a.checkEqual("%'d",     String_t(Format("grp neg dez        = `%'d'") << -4711),    "grp neg dez        = `-4,711'");
}

AFL_TEST("afl.string.Format:int-conditions", a)
{
    static const char* str = "cond = `%d %1{item for %d dollar%!1{s%}%|items for %d dollar%!1{s%} each%}'";

    a.checkEqual("0.0", String_t(Format(str) << 0 << 0), "cond = `0 items for 0 dollars each'");
    a.checkEqual("0.1", String_t(Format(str) << 0 << 1), "cond = `0 items for 1 dollar each'");
    a.checkEqual("0.2", String_t(Format(str) << 0 << 2), "cond = `0 items for 2 dollars each'");
    a.checkEqual("1.0", String_t(Format(str) << 1 << 0), "cond = `1 item for 0 dollars'");
    a.checkEqual("1.1", String_t(Format(str) << 1 << 1), "cond = `1 item for 1 dollar'");
    a.checkEqual("1.2", String_t(Format(str) << 1 << 2), "cond = `1 item for 2 dollars'");
    a.checkEqual("2.0", String_t(Format(str) << 2 << 0), "cond = `2 items for 0 dollars each'");
    a.checkEqual("2.1", String_t(Format(str) << 2 << 1), "cond = `2 items for 1 dollar each'");
    a.checkEqual("2.2", String_t(Format(str) << 2 << 2), "cond = `2 items for 2 dollars each'");
}

/** Test unsigned formats. */
AFL_TEST("afl.string.Format:uint", a)
{
    a.checkEqual("%d",       String_t(Format("dezimal            = `%d'") << 42U),        "dezimal            = `42'");
    a.checkEqual("%x",       String_t(Format("hex                = `%x'") << 42U),        "hex                = `2a'");
    a.checkEqual("%o",       String_t(Format("oct                = `%o'") << 42U),        "oct                = `52'");
    a.checkEqual("%#x",      String_t(Format("alt.hex            = `%#x'") << 42U),       "alt.hex            = `0x2a'");
    a.checkEqual("%#o",      String_t(Format("alt.oct            = `%#o'") << 42U),       "alt.oct            = `052'");
    a.checkEqual("%#d",      String_t(Format("alt.dezimal        = `%#d'") << 42U),       "alt.dezimal        = `42'");
    a.checkEqual("%'d",      String_t(Format("group dez          = `%'d'") << 42U),       "group dez          = `42'");
    a.checkEqual("%'o",      String_t(Format("group oct          = `%'o'") << 42U),       "group oct          = `52'");
    a.checkEqual("%'10d",    String_t(Format("10-dez             = `%10d'") << 42U),      "10-dez             = `        42'");
    a.checkEqual("%-10d",    String_t(Format("left 10-dez        = `%-10d'") << 42U),     "left 10-dez        = `42        '");
    a.checkEqual("%'-10d",   String_t(Format("left group 10-dez  = `%'-10d'") << 42U),    "left group 10-dez  = `42        '");
    a.checkEqual("%'10d",    String_t(Format("group 10-dez       = `%'10d'") << 42U),     "group 10-dez       = `        42'");
    a.checkEqual("%010d",    String_t(Format("zpad 10-dez        = `%010d'") << 42U),     "zpad 10-dez        = `0000000042'");
    a.checkEqual("%+010d",   String_t(Format("sign zpad 10-dez   = `%+010d'") << 42U),    "sign zpad 10-dez   = `+000000042'");
    a.checkEqual("%'10d",    String_t(Format("group 10-dez       = `%'10d'") << 1024U),   "group 10-dez       = `     1,024'");
    a.checkEqual("%'10d 2",  String_t(Format("group 10-dez       = `%'10d'") << 45054U),  "group 10-dez       = `    45,054'");
    a.checkEqual("%'10X",    String_t(Format("group 10-hex       = `%'10X'") << 45054U),  "group 10-hex       = `     A,FFE'");
    a.checkEqual("%#'10X",   String_t(Format("group alt 10-hex   = `%#'10X'") << 45054U), "group alt 10-hex   = `   0XA,FFE'");
    a.checkEqual("%#'10X 2", String_t(Format("group alt 10 dez   = `%#'10X'") << 999U),   "group alt 10 dez   = `     0X3E7'");
}

AFL_TEST("afl.string.Format:uint-conditions", a)
{
    static const char* str = "cond = `%d %1{item for %d dollar%!1{s%}%|items for %d dollar%!1{s%} each%}'";

    a.checkEqual("0.0", String_t(Format(str) << 0U << 0U), "cond = `0 items for 0 dollars each'");
    a.checkEqual("0.1", String_t(Format(str) << 0U << 1U), "cond = `0 items for 1 dollar each'");
    a.checkEqual("0.2", String_t(Format(str) << 0U << 2U), "cond = `0 items for 2 dollars each'");
    a.checkEqual("1.0", String_t(Format(str) << 1U << 0U), "cond = `1 item for 0 dollars'");
    a.checkEqual("1.1", String_t(Format(str) << 1U << 1U), "cond = `1 item for 1 dollar'");
    a.checkEqual("1.2", String_t(Format(str) << 1U << 2U), "cond = `1 item for 2 dollars'");
    a.checkEqual("2.0", String_t(Format(str) << 2U << 0U), "cond = `2 items for 0 dollars each'");
    a.checkEqual("2.1", String_t(Format(str) << 2U << 1U), "cond = `2 items for 1 dollar each'");
    a.checkEqual("2.2", String_t(Format(str) << 2U << 2U), "cond = `2 items for 2 dollars each'");
}

/** Test floating-point formats. */
AFL_TEST("afl.string.Format:fp", a)
{
    /* floating-point tests */
    a.checkEqual("%e",          String_t(Format("Format e           = `%e'") << 4711.0),     "Format e           = `4.711000e+03'");
    a.checkEqual("%f",          String_t(Format("Format f           = `%f'") << 4711.0),     "Format f           = `4711.000000'");
    a.checkEqual("%g",          String_t(Format("Format g           = `%g'") << 4711.0),     "Format g           = `4711'");
    a.checkEqual("%e one",      String_t(Format("Format e one       = `%e'") << 1.0),        "Format e one       = `1.000000e+00'");
    a.checkEqual("%f one",      String_t(Format("Format f one       = `%f'") << 1.0),        "Format f one       = `1.000000'");
    a.checkEqual("%g one",      String_t(Format("Format g one       = `%g'") << 1.0),        "Format g one       = `1'");
    a.checkEqual("%e zero",     String_t(Format("Format e zero      = `%e'") << 0.0),        "Format e zero      = `0.000000e+00'");
    a.checkEqual("%f zero",     String_t(Format("Format f zero      = `%f'") << 0.0),        "Format f zero      = `0.000000'");
    a.checkEqual("%g zero",     String_t(Format("Format g zero      = `%g'") << 0.0),        "Format g zero      = `0'");
    a.checkEqual("%e tiny",     String_t(Format("Format e tiny      = `%e'") << 0.00000123), "Format e tiny      = `1.230000e-06'");
    a.checkEqual("%f tiny",     String_t(Format("Format f tiny      = `%f'") << 0.00000123), "Format f tiny      = `0.000001'");
    a.checkEqual("%g tiny",     String_t(Format("Format g tiny      = `%g'") << 0.00000123), "Format g tiny      = `1.23e-06'");
    a.checkEqual("%.2g",        String_t(Format("Format .2g         = `%.2g'") << 12345.0),  "Format .2g         = `1.23e+04'");

    /* more floating-point tests */
    a.checkEqual("%.2f .999",   String_t(Format("Format .2f .999     = `%.2f'") << 0.999),   "Format .2f .999     = `1.00'");
    a.checkEqual("%.2e .999",   String_t(Format("Format .2e .999     = `%.2e'") << 0.999),   "Format .2e .999     = `9.99e-01'");
    a.checkEqual("%.2f 99.999", String_t(Format("Format .2f 99.999   = `%.2f'") << 99.999),  "Format .2f 99.999   = `100.00'");
    a.checkEqual("%.2e 99.999", String_t(Format("Format .2e 99.999   = `%.2e'") << 99.999),  "Format .2e 99.999   = `1.00e+02'");
    a.checkEqual("%.5f 99.999", String_t(Format("Format .5f 99.999   = `%.5f'") << 99.999),  "Format .5f 99.999   = `99.99900'");
    a.checkEqual("%.5e 99.999", String_t(Format("Format .5e 99.999   = `%.5e'") << 99.999),  "Format .5e 99.999   = `9.99990e+01'");
    a.checkEqual("%.2f 25",     String_t(Format("Format .2f 25       = `%.2f'") << 25.0),    "Format .2f 25       = `25.00'");
    a.checkEqual("%.2e 25",     String_t(Format("Format .2e 25       = `%.2e'") << 25.0),    "Format .2e 25       = `2.50e+01'");
    a.checkEqual("%.2f .25",    String_t(Format("Format .2f .25      = `%.2f'") << 0.25),    "Format .2f .25      = `0.25'");
    a.checkEqual("%.2e .25",    String_t(Format("Format .2e .25      = `%.2e'") << 0.25),    "Format .2e .25      = `2.50e-01'");

    a.checkEqual("%g 100",      String_t(Format("Format g hundred    = `%g'") << 100.0),     "Format g hundred    = `100'");
    a.checkEqual("%.0f 100",    String_t(Format("Format f hundred    = `%.0f'") << 100.0),   "Format f hundred    = `100'");
    a.checkEqual("%.1f 100",    String_t(Format("Format f hundred    = `%.1f'") << 100.0),   "Format f hundred    = `100.0'");

    a.checkEqual("%.1f small",  String_t(Format("%.1f", 0.003003003)), "0.0");
    a.checkEqual("%.1f 0.99",   String_t(Format("%.1f", 0.99)), "1.0");
    a.checkEqual("%.1f 99.99",  String_t(Format("%.1f", 99.99)), "100.0");

    // Nonfinite
    a.checkEqual("%.1f Inf",    String_t(Format("%.1f", std::exp(100000))), "Inf");
    a.checkEqual("%.1f -Inf",   String_t(Format("%.1f", -std::exp(100000))), "-Inf");
}

/** Test that strings formatted as integers set flags. */
AFL_TEST("afl.string.Format:string-flags", a)
{
    static const char* str = "cond = `%d %1{item for %d dollar%!1{s%}%|items for %d dollar%!1{s%} each%}'";

    // Using const char*:
    a.checkEqual("0.0 charp", String_t(Format(str) << "0" << "0"), "cond = `0 items for 0 dollars each'");
    a.checkEqual("0.1 charp", String_t(Format(str) << "0" << "1"), "cond = `0 items for 1 dollar each'");
    a.checkEqual("0.2 charp", String_t(Format(str) << "0" << "2"), "cond = `0 items for 2 dollars each'");
    a.checkEqual("1.0 charp", String_t(Format(str) << "1" << "0"), "cond = `1 item for 0 dollars'");
    a.checkEqual("1.1 charp", String_t(Format(str) << "1" << "1"), "cond = `1 item for 1 dollar'");
    a.checkEqual("1.2 charp", String_t(Format(str) << "1" << "2"), "cond = `1 item for 2 dollars'");
    a.checkEqual("2.0 charp", String_t(Format(str) << "2" << "0"), "cond = `2 items for 0 dollars each'");
    a.checkEqual("2.1 charp", String_t(Format(str) << "2" << "1"), "cond = `2 items for 1 dollar each'");
    a.checkEqual("2.2 charp", String_t(Format(str) << "2" << "2"), "cond = `2 items for 2 dollars each'");

    a.checkEqual("000.000 charp", String_t(Format(str) << "0000000000000" << "0000000000000"), "cond = `0000000000000 items for 0000000000000 dollars each'");
    a.checkEqual("000.001 charp", String_t(Format(str) << "0000000000000" << "0000000000001"), "cond = `0000000000000 items for 0000000000001 dollar each'");
    a.checkEqual("000.002 charp", String_t(Format(str) << "0000000000000" << "0000000000002"), "cond = `0000000000000 items for 0000000000002 dollars each'");
    a.checkEqual("01x.nnn charp", String_t(Format(str) << "000000000001x" << "0004294967297"), "cond = `000000000001x item for 0004294967297 dollars'");

    a.checkEqual("", String_t(Format(str) << "000,000,000,1" << ",,,,,,0,,0,,,"), "cond = `000,000,000,1 item for ,,,,,,0,,0,,, dollars'");

    // Using String_t:
    a.checkEqual("0.0 str", String_t(Format(str) << String_t("0") << String_t("0")), "cond = `0 items for 0 dollars each'");
    a.checkEqual("0.1 str", String_t(Format(str) << String_t("0") << String_t("1")), "cond = `0 items for 1 dollar each'");
    a.checkEqual("0.2 str", String_t(Format(str) << String_t("0") << String_t("2")), "cond = `0 items for 2 dollars each'");
    a.checkEqual("1.0 str", String_t(Format(str) << String_t("1") << String_t("0")), "cond = `1 item for 0 dollars'");
    a.checkEqual("1.1 str", String_t(Format(str) << String_t("1") << String_t("1")), "cond = `1 item for 1 dollar'");
    a.checkEqual("1.2 str", String_t(Format(str) << String_t("1") << String_t("2")), "cond = `1 item for 2 dollars'");
    a.checkEqual("2.0 str", String_t(Format(str) << String_t("2") << String_t("0")), "cond = `2 items for 0 dollars each'");
    a.checkEqual("2.1 str", String_t(Format(str) << String_t("2") << String_t("1")), "cond = `2 items for 1 dollar each'");
    a.checkEqual("2.2 str", String_t(Format(str) << String_t("2") << String_t("2")), "cond = `2 items for 2 dollars each'");

    a.checkEqual("000.000 str", String_t(Format(str) << String_t("0000000000000") << String_t("0000000000000")), "cond = `0000000000000 items for 0000000000000 dollars each'");
    a.checkEqual("000.001 str", String_t(Format(str) << String_t("0000000000000") << String_t("0000000000001")), "cond = `0000000000000 items for 0000000000001 dollar each'");
    a.checkEqual("000.002 str", String_t(Format(str) << String_t("0000000000000") << String_t("0000000000002")), "cond = `0000000000000 items for 0000000000002 dollars each'");
    a.checkEqual("01x.nnn str", String_t(Format(str) << String_t("000000000001x") << String_t("0004294967297")), "cond = `000000000001x item for 0004294967297 dollars'");

    String_t one = "11";
    a.checkEqual("11 str", String_t(Format(str) << one << one), "cond = `11 items for 11 dollars each'");
    one.erase(1);
    a.checkEqual("1 str", String_t(Format(str) << one << one), "cond = `1 item for 1 dollar'");
}

/** Test ability to reorder parameters. */
AFL_TEST("afl.string.Format:reorder", a)
{
    a.checkEqual("1 arg", String_t(Format("%d-%d-%d",       1, 2, 3)), "1-2-3");
    a.checkEqual("2 arg", String_t(Format("%2$d-%1$d-%0$d", 1, 2, 3)), "3-2-1");
    a.checkEqual("3 arg", String_t(Format("%1$d-%d-%0$d",   1, 2, 3)), "2-3-1");

    a.checkEqual("1 shift", String_t(Format("%d-%d-%d")       << 1 << 2 << 3), "1-2-3");
    a.checkEqual("2 shift", String_t(Format("%2$d-%1$d-%0$d") << 1 << 2 << 3), "3-2-1");
    a.checkEqual("3 shift", String_t(Format("%1$d-%d-%0$d")   << 1 << 2 << 3), "2-3-1");
}

/** Test streaming conversion. */
AFL_TEST("afl.string.Format:stream", a)
{
    // // Constructor operation
    // {
    //     std::stringstream ss;
    //     ss << "hi " << Format("a%db", 10) << " ho";
    //     a.checkEqual("", ss.str(), "hi a10b ho");
    // }

    // // This one has wrong associativity!
    // {
    //     std::stringstream ss;
    //     ss << "hi " << Format("a%db") << 10 << " ho";
    //     a.checkEqual("", ss.str(), "hi a<invalid>b10 ho");
    // }

    // Inserter operation
    {
        std::stringstream ss;
        ss << "hi " << (Format("a%db") << 10) << " ho";
        a.checkEqual("", ss.str(), "hi a10b ho");
    }
}

/** Test some format string specialties. */
AFL_TEST("afl.string.Format:specials", a)
{
    // Partial placeholder
    a.checkEqual("partial 1", String_t(Format("%")), "");
    a.checkEqual("partial 2", String_t(Format("%10")), "");
    a.checkEqual("partial 3", String_t(Format("%10.")), "");
    a.checkEqual("partial 4", String_t(Format("% ")), "");

    // Percent
    a.checkEqual("literal 1", String_t(Format("%%")), "%");
    a.checkEqual("literal 2", String_t(Format("%5%")), "    %");
}

/** Test pointers. */
AFL_TEST("afl.string.Format:pointer", a)
{
    // Null pointer
    a.checkEqual("null-void-as-ptr", String_t(Format("%p", (void*) 0)), "(nil)");
    a.checkEqual("null-char-as-ptr", String_t(Format("%p", (const char*) 0)), "(nil)");
    a.checkEqual("null-char-as-str", String_t(Format("%s", (const char*) 0)), "(nil)");

    // Non-null pointer
    a.checkEqual("char-as-ptr", String_t(Format("%p", "xyz")).substr(0, 2), "0x");
    a.checkEqual("void-as-str", String_t(Format("%s", (void*) "xyz")).substr(0, 2), "0x");
    int tmp;
    a.checkEqual("int-as-str", String_t(Format("%s", &tmp)).substr(0, 2), "0x");
}
