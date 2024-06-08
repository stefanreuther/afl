/**
  *  \file afl/test/makeprintable.hpp
  *  \brief Function afl::test::makePrintable
  */
#ifndef AFL_AFL_TEST_MAKEPRINTABLE_HPP
#define AFL_AFL_TEST_MAKEPRINTABLE_HPP

namespace afl { namespace test {

    /** @fn makePrintable

        @brief Making Things Printable

        Whereas Format() is pretty strict with formatting, we want to be a bit more relaxed in assertion failure formatting,
        and we want to be able to differentiate between "production printout" and "test printout" formats.

        Function makePrintable(x) is supposed to return x in a way that is accepted by Format("%Q").
        Unlike Format(), it will employ C++ type conversions.
        In particular, this makes output of enum values work.

        The framework provides overloads for all types accepted by Format() by default.
        (We therefore re-use some of Format's internals.)

        To make own types printable, supply your own overloads. */

#define AFL_COMMA ,
#define AFL_TEST_DEFINE_MAKE_PRINTABLE(T) \
    inline T makePrintable(T v) { return v; }

    AFL_TEST_DEFINE_MAKE_PRINTABLE(const String_t&)
    AFL_TEST_DEFINE_MAKE_PRINTABLE(const char*)
    AFL_TEST_DEFINE_MAKE_PRINTABLE(const void*)
    AFL_TEST_DEFINE_MAKE_PRINTABLE(float)
    AFL_TEST_DEFINE_MAKE_PRINTABLE(double)
    AFL_TEST_DEFINE_MAKE_PRINTABLE(long double)

    AFL_TEST_DEFINE_MAKE_PRINTABLE(unsigned char)
    AFL_TEST_DEFINE_MAKE_PRINTABLE(unsigned short)
    AFL_TEST_DEFINE_MAKE_PRINTABLE(unsigned int)
    AFL_TEST_DEFINE_MAKE_PRINTABLE(unsigned long)
    AFL_TEST_DEFINE_MAKE_PRINTABLE(unsigned long long)

    AFL_TEST_DEFINE_MAKE_PRINTABLE(const afl::string::detail::FormatSelect<afl::string::detail::UnsignedTypes1 AFL_COMMA struct FormatSizet    AFL_COMMA size_t>::Type&)
    AFL_TEST_DEFINE_MAKE_PRINTABLE(const afl::string::detail::FormatSelect<afl::string::detail::UnsignedTypes2 AFL_COMMA struct FormatUintptrt AFL_COMMA uintptr_t>::Type&)
    AFL_TEST_DEFINE_MAKE_PRINTABLE(const afl::string::detail::FormatSelect<afl::string::detail::UnsignedTypes3 AFL_COMMA struct FormatUintmaxt AFL_COMMA uintmax_t>::Type&)
    AFL_TEST_DEFINE_MAKE_PRINTABLE(const afl::string::detail::FormatSelect<afl::string::detail::UnsignedTypes4 AFL_COMMA struct FormatU8       AFL_COMMA uint8_t >::Type&)
    AFL_TEST_DEFINE_MAKE_PRINTABLE(const afl::string::detail::FormatSelect<afl::string::detail::UnsignedTypes4 AFL_COMMA struct FormatU16      AFL_COMMA uint16_t>::Type&)
    AFL_TEST_DEFINE_MAKE_PRINTABLE(const afl::string::detail::FormatSelect<afl::string::detail::UnsignedTypes4 AFL_COMMA struct FormatU32      AFL_COMMA uint32_t>::Type&)
    AFL_TEST_DEFINE_MAKE_PRINTABLE(const afl::string::detail::FormatSelect<afl::string::detail::UnsignedTypes4 AFL_COMMA struct FormatU64      AFL_COMMA uint64_t>::Type&)

    AFL_TEST_DEFINE_MAKE_PRINTABLE(char)
    AFL_TEST_DEFINE_MAKE_PRINTABLE(signed char)
    AFL_TEST_DEFINE_MAKE_PRINTABLE(signed short)
    AFL_TEST_DEFINE_MAKE_PRINTABLE(signed int)
    AFL_TEST_DEFINE_MAKE_PRINTABLE(signed long)
    AFL_TEST_DEFINE_MAKE_PRINTABLE(signed long long)

    AFL_TEST_DEFINE_MAKE_PRINTABLE(const afl::string::detail::FormatSelect<afl::string::detail::SignedTypes1 AFL_COMMA struct FormatPtrdifft AFL_COMMA ptrdiff_t>::Type&)
    AFL_TEST_DEFINE_MAKE_PRINTABLE(const afl::string::detail::FormatSelect<afl::string::detail::SignedTypes2 AFL_COMMA struct FormatIntptrt  AFL_COMMA intptr_t>::Type&)
    AFL_TEST_DEFINE_MAKE_PRINTABLE(const afl::string::detail::FormatSelect<afl::string::detail::SignedTypes3 AFL_COMMA struct FormatIntmaxt  AFL_COMMA intmax_t>::Type&)
    AFL_TEST_DEFINE_MAKE_PRINTABLE(const afl::string::detail::FormatSelect<afl::string::detail::SignedTypes4 AFL_COMMA struct FormatI8       AFL_COMMA int8_t >::Type&)
    AFL_TEST_DEFINE_MAKE_PRINTABLE(const afl::string::detail::FormatSelect<afl::string::detail::SignedTypes4 AFL_COMMA struct FormatI16      AFL_COMMA int16_t>::Type&)
    AFL_TEST_DEFINE_MAKE_PRINTABLE(const afl::string::detail::FormatSelect<afl::string::detail::SignedTypes4 AFL_COMMA struct FormatI32      AFL_COMMA int32_t>::Type&)
    AFL_TEST_DEFINE_MAKE_PRINTABLE(const afl::string::detail::FormatSelect<afl::string::detail::SignedTypes4 AFL_COMMA struct FormatI64      AFL_COMMA int64_t>::Type&)
#undef AFL_COMMA

} }

#endif
