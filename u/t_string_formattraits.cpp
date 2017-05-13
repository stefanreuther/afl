/**
  *  \file u/t_string_formattraits.cpp
  *  \brief Test for afl::string::FormatTraits
  */

#include "afl/string/formattraits.hpp"
#include "u/t_string.hpp"
#include "afl/string/formatunion.hpp"

/** Test specialisation on all types.
    All of these must compile.
    Proper operation is tested later on in TestStringFormat::testTypes(). */
void
TestStringFormatTraits::testTypes()
{
    afl::string::FormatUnion u;

    // uintX_t and intX_t
    afl::string::FormatTraits<uint8_t>::store(u, 1);
    afl::string::FormatTraits<uint16_t>::store(u, 1);
    afl::string::FormatTraits<uint32_t>::store(u, 1);
    afl::string::FormatTraits<uint64_t>::store(u, 1);
    afl::string::FormatTraits<uintmax_t>::store(u, 1);
    afl::string::FormatTraits<uintptr_t>::store(u, 1);

    afl::string::FormatTraits<int8_t>::store(u, 1);
    afl::string::FormatTraits<int16_t>::store(u, 1);
    afl::string::FormatTraits<int32_t>::store(u, 1);
    afl::string::FormatTraits<int64_t>::store(u, 1);
    afl::string::FormatTraits<intmax_t>::store(u, 1);
    afl::string::FormatTraits<intptr_t>::store(u, 1);

    // built-ins
    afl::string::FormatTraits<short int>::store(u, 1);
    afl::string::FormatTraits<int>::store(u, 1);
    afl::string::FormatTraits<long int>::store(u, 1);
    afl::string::FormatTraits<long long int>::store(u, 1);

    afl::string::FormatTraits<unsigned short int>::store(u, 1);
    afl::string::FormatTraits<unsigned int>::store(u, 1);
    afl::string::FormatTraits<unsigned long int>::store(u, 1);
    afl::string::FormatTraits<unsigned long long int>::store(u, 1);

    // size
    afl::string::FormatTraits<size_t>::store(u, sizeof(u));

    // other
    afl::string::FormatTraits<const char*>::store(u, "hi there");
    afl::string::FormatTraits<const void*>::store(u, (void*) 0);
    afl::string::FormatTraits<const void*>::store(u, (void*) &u);
}
