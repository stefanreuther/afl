/**
  *  \file afl/charset/codepage.hpp
  *  \brief Class afl::charset::Codepage
  */
#ifndef AFL_AFL_CHARSET_CODEPAGE_HPP
#define AFL_AFL_CHARSET_CODEPAGE_HPP

#include "afl/base/types.hpp"

namespace afl { namespace charset {

    /** Definition of a codepage.
        In our definition, codepages are 256-byte character sets,
        where the lower half is ASCII, the upper half is arbitrary characters.
        A structure of this type defines a codepage by listing the upper half.

        Since none of our codepages requires characters beyond the BMP,
        we save memory by using just 16 bit values. */
    struct Codepage {
        /** Character codes (Unicode codepoints).
            The first index corresponds to character code 0x80. */
        uint16_t m_characters[128];
    };

    /** Windows Codepage 1250 (Eastern Europe). */
    extern const Codepage g_codepage1250;

    /** Windows Codepage 1251 (Cyrillic). */
    extern const Codepage g_codepage1251;

    /** Windows Codepage 1252 (Western Europe, Latin-1). */
    extern const Codepage g_codepage1252;

    /** MS-DOS Codepage 437 (Western Europe). */
    extern const Codepage g_codepage437;

    /** MS-DOS Codepage 850 (Central Europe). */
    extern const Codepage g_codepage850;

    /** MS-DOS Codepage 852 (Eastern Europe). */
    extern const Codepage g_codepage852;

    /** MS-DOS Codepage 866 (Cyrillic). */
    extern const Codepage g_codepage866;

    /** Cyrillic codepage (KOI8-R). */
    extern const Codepage g_codepageKOI8R;

    /** ISO 8859-1 (Latin-1). */
    extern const Codepage g_codepageLatin1;

    /** ISO 8859-2 (Latin-2). */
    extern const Codepage g_codepageLatin2;

} }

#endif
