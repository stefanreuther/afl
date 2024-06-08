/**
  *  \file afl/charset/utf8.hpp
  *  \brief Class afl::charset::Utf8
  */
#ifndef AFL_AFL_CHARSET_UTF8_HPP
#define AFL_AFL_CHARSET_UTF8_HPP

#include "afl/string/string.hpp"
#include "afl/charset/unicode.hpp"

namespace afl { namespace charset {

    /** UTF-8 String Utilities.
        This class provides various utilities for working with UTF-8 strings.
        UTF-8 strings consist of <b>runes</b>, each of which can occupy one or more bytes.

        Each class instance is configured with a set of flags (AllowXxxx) that specify
        which precise flavour of UTF-8 you'll be dealing with.

        In particular, this means
        - we support the modified UTF-8 forms used in Java, for example ("CESU-8"),
          using the AllowEncodedZero and AllowSurrogates flags.
        - we support decoding invalid UTF-8 (AllowNonMinimal, AllowNonCharacters).
        - we support round-trip decoding of strings-we-think-are-UTF-8-which-actually-aren't (AllowErrorEscapes).

        When decoding, invalid runes yield error characters.
        This means that, when AllowErrorEscapes is not set,
        it's impossible to distinguish an invalid rune from a rune representing an error character.

        Note that although this class has charAt() and length() functions, do not use
        <pre>
           for (i = 0; i < u8.length(s); ++i) {   // WRONG!
               foo(u8.charAt(s, i));              // WRONG!
           }                                      // WRONG!
        </pre>
        This code is O(n^2). Use Utf8Reader instead. */
    class Utf8 {
     public:
        /*
         *  Helper Typedefs
         */

        /** Type of memory. We decode raw memory. */
        typedef afl::base::ConstBytes_t Bytes_t;

        /*
         *  Conversion Flags
         */

        /** Allow encoded zero.
            - Decoding:
              - If this is set, U+0000 encoded as two bytes (0xC0 0x80) is accepted.
              - If this is not set, AllowNonMinimal applies.
            - Encoding:
              - If this is set, U+0000 is encoded as two bytes.
              - If this is not set, U+0000 is encoded as one byte. */
        static const uint32_t AllowEncodedZero = 1;

        /** Allow encoded surrogates.
            - Decoding:
              - If this is set, encoded surrogates are decoded.
              - If this is not set, AllowNonCharacters handling applies.
            - Encoding:
              - If this is set, non-BMP characters are encoded as surrogate halves ("CESU-8").
              - If this is not set, non-BMP characters are encoded as-is. */
        static const uint32_t AllowSurrogates = 2;

        /** Allow non-minimal encodings.
            - Decoding:
              - If this is specified, non-minimally encoded characters are accepted.
              - If this is not set, an error is returned.
              - Encoding: not relevant; encoding always produces minimal results (except for AllowEncodedZero). */
        static const uint32_t AllowNonMinimal = 4;

        /** Allow non-characters.
            If this is set during decoding, non-characters are accepted.
            Non-characters are valid encoded runes that produce
            - characters outside the Unicode range [0, UNICODE_MAX]
            - characters 0xFFFE and 0xFFFF in every plane
            - surrogate halves

            For encoding, this is not relevant. */
        static const uint32_t AllowNonCharacters = 8;

        /** Allow error-characters.
            - Decoding:
              - If this is set, error characters are treated as errors, and each individual byte produces an error character
              - If this is not set, error characters are returned a single error character
            - Encoding:
              - If this is set, error characters produce the erroneous character
              - If this is not set, error characters are encoded as-is */
        static const uint32_t AllowErrorEscapes = 16;


        /*
         *  Methods
         */

        /** Constructor.
            \param flags Flags for this instance. */
        explicit Utf8(uint32_t flags = 0);

        /** Append UTF-8 character.
            \param s [in/out] String to work on
            \param ch [in] Unicode character
            \return s */
        String_t& append(String_t& s, Unichar_t ch) const;

        /** Compute number of characters.
            \param text [in] String to work on
            \return Number of UTF-8 runes in string */
        size_t length(const String_t& text) const;

        /** Compute number of characters.
            \param text [in] String to work on
            \return Number of UTF-8 runes in string */
        size_t length(Bytes_t text) const;

        /** Get substring.
            \param text [in] String to work on
            \param index [in] Index of first rune to extract, 0-based
            \param count [in] Maximum number of runes to extract
            \return Substring. Empty if index or count are out of range. */
        String_t substr(const String_t& text, size_t index, size_t count) const;

        /** Get substring.
            \param text [in] String to work on
            \param index [in] Index of first rune to extract, 0-based
            \param count [in] Maximum number of runes to extract
            \return Substring. Empty if index or count are out of range. */
        String_t substr(Bytes_t text, size_t index, size_t count) const;

        /** Get substring.
            \param text [in] String to work on
            \param index [in] Index of first rune to extract, 0-based
            \param count [in] Maximum number of runes to extract
            \return Substring as Memory object referring into \c text. Empty if index or count are out of range. */
        Bytes_t substrMemory(Bytes_t text, size_t index, size_t count) const;

        /** Get character by position.
            \param text [in] String to work on
            \param index [in] Index of rune to extract
            \return Character, 0 if index is out of range. */
        Unichar_t charAt(const String_t& text, size_t index) const;

        /** Get character by position.
            \param text [in] String to work on
            \param index [in] Index of rune to extract
            \return Character, 0 if index is out of range. */
        Unichar_t charAt(Bytes_t text, size_t index) const;

        /** Convert byte position to character position.
            \param text [in] String to work on
            \param bytePos [in] Byte position. Should point into text.
            \return Number of characters before bytePos. If bytePos points outside the text, number of characters total. */
        size_t byteToCharPos(const String_t& text, size_t bytePos) const;

        /** Convert byte position to character position.
            \param text [in] String to work on
            \param bytePos [in] Byte position. Should point into text.
            \return Number of characters before bytePos. If bytePos points outside the text, number of characters total. */
        size_t byteToCharPos(Bytes_t text, size_t bytePos) const;

        /** Convert character position to byte position.
            \param text [in] String to work on
            \param charPos [in] Character position
            \return Number of bytes that make up the first charPos characters. Number of bytes in text if there are fewer characters in the text. */
        size_t charToBytePos(const String_t& text, size_t charPos) const;

        /** Convert character position to byte position.
            \param text [in] String to work on
            \param charPos [in] Character position
            \return Number of bytes that make up the first charPos characters. Number of bytes in text if there are fewer characters in the text. */
        size_t charToBytePos(Bytes_t text, size_t charPos) const;

        /** Check for continuation byte.
            \param byte Byte to check
            \retval false Not a continuation byte. This byte can probably start an UTF-8 rune.
            \retval true Continuation byte. This byte cannot start an UTF-8 rune. */
        static bool isContinuationByte(uint8_t byte);

     private:
        /** UTF-8 encoder/decoder flags. */
        uint32_t m_flags;
    };

} }

#endif
