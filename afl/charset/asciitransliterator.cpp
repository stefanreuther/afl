/**
  *  \file afl/charset/asciitransliterator.cpp
  *  \brief Class afl::charset::AsciiTransliterator
  */

#include <cassert>
#include "afl/charset/asciitransliterator.hpp"
#include "afl/charset/utf8reader.hpp"

namespace {
    const size_t MAX_CHARS = 4;
    struct Element {
        uint16_t code;
        char transliteration[MAX_CHARS];
    };

    /*
     *  Transliteration table:
     *
     *  Data has been taken from a project "Unidecode", in its Java incarnation,
     *  available here: https://github.com/xuender/unidecode, under the license below.
     *
     *  Data has been limited to U+0080..U+05FF, U+1D00..U+2FFF (which is the same
     *  range as covered by getUpperCase()/getLowerCase(), character categories
     *  Lu/Ll/Lt/Lm/Lo/Sm/Sc/Nd/No.
     *
     *  ----
     *
     *  The MIT License (MIT)
     *
     *  Copyright (c) 2015 ender
     *
     *  Permission is hereby granted, free of charge, to any person obtaining a copy
     *  of this software and associated documentation files (the "Software"), to deal
     *  in the Software without restriction, including without limitation the rights
     *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
     *  copies of the Software, and to permit persons to whom the Software is
     *  furnished to do so, subject to the following conditions:
     *
     *  The above copyright notice and this permission notice shall be included in all
     *  copies or substantial portions of the Software.
     *
     *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
     *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
     *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
     *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
     *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
     *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
     *  SOFTWARE.
     */
    static const Element TRANSLITERATION[] = {
        { 0x00A2, { 'C', '/', 0, 0 } },                // CENT SIGN
        { 0x00A3, { 'P', 'S', 0, 0 } },                // POUND SIGN
        { 0x00A4, { '$', '?', 0, 0 } },                // CURRENCY SIGN
        { 0x00A5, { 'Y', '=', 0, 0 } },                // YEN SIGN
        { 0x00AA, { 'a', 0, 0, 0 } },                  // FEMININE ORDINAL INDICATOR
        { 0x00AC, { '!', 0, 0, 0 } },                  // NOT SIGN
        { 0x00B1, { '+', '-', 0, 0 } },                // PLUS-MINUS SIGN
        { 0x00B2, { '2', 0, 0, 0 } },                  // SUPERSCRIPT TWO
        { 0x00B3, { '3', 0, 0, 0 } },                  // SUPERSCRIPT THREE
        { 0x00B5, { 'u', 0, 0, 0 } },                  // MICRO SIGN
        { 0x00B9, { '1', 0, 0, 0 } },                  // SUPERSCRIPT ONE
        { 0x00BA, { 'o', 0, 0, 0 } },                  // MASCULINE ORDINAL INDICATOR
        { 0x00BC, { '1', '/', '4', 0 } },              // VULGAR FRACTION ONE QUARTER
        { 0x00BD, { '1', '/', '2', 0 } },              // VULGAR FRACTION ONE HALF
        { 0x00BE, { '3', '/', '4', 0 } },              // VULGAR FRACTION THREE QUARTERS
        { 0x00C0, { 'A', 0, 0, 0 } },                  // LATIN CAPITAL LETTER A WITH GRAVE
        { 0x00C1, { 'A', 0, 0, 0 } },                  // LATIN CAPITAL LETTER A WITH ACUTE
        { 0x00C2, { 'A', 0, 0, 0 } },                  // LATIN CAPITAL LETTER A WITH CIRCUMFLEX
        { 0x00C3, { 'A', 0, 0, 0 } },                  // LATIN CAPITAL LETTER A WITH TILDE
        { 0x00C4, { 'A', 0, 0, 0 } },                  // LATIN CAPITAL LETTER A WITH DIAERESIS
        { 0x00C5, { 'A', 0, 0, 0 } },                  // LATIN CAPITAL LETTER A WITH RING ABOVE
        { 0x00C6, { 'A', 'E', 0, 0 } },                // LATIN CAPITAL LETTER AE
        { 0x00C7, { 'C', 0, 0, 0 } },                  // LATIN CAPITAL LETTER C WITH CEDILLA
        { 0x00C8, { 'E', 0, 0, 0 } },                  // LATIN CAPITAL LETTER E WITH GRAVE
        { 0x00C9, { 'E', 0, 0, 0 } },                  // LATIN CAPITAL LETTER E WITH ACUTE
        { 0x00CA, { 'E', 0, 0, 0 } },                  // LATIN CAPITAL LETTER E WITH CIRCUMFLEX
        { 0x00CB, { 'E', 0, 0, 0 } },                  // LATIN CAPITAL LETTER E WITH DIAERESIS
        { 0x00CC, { 'I', 0, 0, 0 } },                  // LATIN CAPITAL LETTER I WITH GRAVE
        { 0x00CD, { 'I', 0, 0, 0 } },                  // LATIN CAPITAL LETTER I WITH ACUTE
        { 0x00CE, { 'I', 0, 0, 0 } },                  // LATIN CAPITAL LETTER I WITH CIRCUMFLEX
        { 0x00CF, { 'I', 0, 0, 0 } },                  // LATIN CAPITAL LETTER I WITH DIAERESIS
        { 0x00D0, { 'D', 0, 0, 0 } },                  // LATIN CAPITAL LETTER ETH
        { 0x00D1, { 'N', 0, 0, 0 } },                  // LATIN CAPITAL LETTER N WITH TILDE
        { 0x00D2, { 'O', 0, 0, 0 } },                  // LATIN CAPITAL LETTER O WITH GRAVE
        { 0x00D3, { 'O', 0, 0, 0 } },                  // LATIN CAPITAL LETTER O WITH ACUTE
        { 0x00D4, { 'O', 0, 0, 0 } },                  // LATIN CAPITAL LETTER O WITH CIRCUMFLEX
        { 0x00D5, { 'O', 0, 0, 0 } },                  // LATIN CAPITAL LETTER O WITH TILDE
        { 0x00D6, { 'O', 0, 0, 0 } },                  // LATIN CAPITAL LETTER O WITH DIAERESIS
        { 0x00D7, { 'x', 0, 0, 0 } },                  // MULTIPLICATION SIGN
        { 0x00D8, { 'O', 0, 0, 0 } },                  // LATIN CAPITAL LETTER O WITH STROKE
        { 0x00D9, { 'U', 0, 0, 0 } },                  // LATIN CAPITAL LETTER U WITH GRAVE
        { 0x00DA, { 'U', 0, 0, 0 } },                  // LATIN CAPITAL LETTER U WITH ACUTE
        { 0x00DB, { 'U', 0, 0, 0 } },                  // LATIN CAPITAL LETTER U WITH CIRCUMFLEX
        { 0x00DC, { 'U', 0, 0, 0 } },                  // LATIN CAPITAL LETTER U WITH DIAERESIS
        { 0x00DD, { 'Y', 0, 0, 0 } },                  // LATIN CAPITAL LETTER Y WITH ACUTE
        { 0x00DE, { 'T', 'h', 0, 0 } },                // LATIN CAPITAL LETTER THORN
        { 0x00DF, { 's', 's', 0, 0 } },                // LATIN SMALL LETTER SHARP S
        { 0x00E0, { 'a', 0, 0, 0 } },                  // LATIN SMALL LETTER A WITH GRAVE
        { 0x00E1, { 'a', 0, 0, 0 } },                  // LATIN SMALL LETTER A WITH ACUTE
        { 0x00E2, { 'a', 0, 0, 0 } },                  // LATIN SMALL LETTER A WITH CIRCUMFLEX
        { 0x00E3, { 'a', 0, 0, 0 } },                  // LATIN SMALL LETTER A WITH TILDE
        { 0x00E4, { 'a', 0, 0, 0 } },                  // LATIN SMALL LETTER A WITH DIAERESIS
        { 0x00E5, { 'a', 0, 0, 0 } },                  // LATIN SMALL LETTER A WITH RING ABOVE
        { 0x00E6, { 'a', 'e', 0, 0 } },                // LATIN SMALL LETTER AE
        { 0x00E7, { 'c', 0, 0, 0 } },                  // LATIN SMALL LETTER C WITH CEDILLA
        { 0x00E8, { 'e', 0, 0, 0 } },                  // LATIN SMALL LETTER E WITH GRAVE
        { 0x00E9, { 'e', 0, 0, 0 } },                  // LATIN SMALL LETTER E WITH ACUTE
        { 0x00EA, { 'e', 0, 0, 0 } },                  // LATIN SMALL LETTER E WITH CIRCUMFLEX
        { 0x00EB, { 'e', 0, 0, 0 } },                  // LATIN SMALL LETTER E WITH DIAERESIS
        { 0x00EC, { 'i', 0, 0, 0 } },                  // LATIN SMALL LETTER I WITH GRAVE
        { 0x00ED, { 'i', 0, 0, 0 } },                  // LATIN SMALL LETTER I WITH ACUTE
        { 0x00EE, { 'i', 0, 0, 0 } },                  // LATIN SMALL LETTER I WITH CIRCUMFLEX
        { 0x00EF, { 'i', 0, 0, 0 } },                  // LATIN SMALL LETTER I WITH DIAERESIS
        { 0x00F0, { 'd', 0, 0, 0 } },                  // LATIN SMALL LETTER ETH
        { 0x00F1, { 'n', 0, 0, 0 } },                  // LATIN SMALL LETTER N WITH TILDE
        { 0x00F2, { 'o', 0, 0, 0 } },                  // LATIN SMALL LETTER O WITH GRAVE
        { 0x00F3, { 'o', 0, 0, 0 } },                  // LATIN SMALL LETTER O WITH ACUTE
        { 0x00F4, { 'o', 0, 0, 0 } },                  // LATIN SMALL LETTER O WITH CIRCUMFLEX
        { 0x00F5, { 'o', 0, 0, 0 } },                  // LATIN SMALL LETTER O WITH TILDE
        { 0x00F6, { 'o', 0, 0, 0 } },                  // LATIN SMALL LETTER O WITH DIAERESIS
        { 0x00F7, { '/', 0, 0, 0 } },                  // DIVISION SIGN
        { 0x00F8, { 'o', 0, 0, 0 } },                  // LATIN SMALL LETTER O WITH STROKE
        { 0x00F9, { 'u', 0, 0, 0 } },                  // LATIN SMALL LETTER U WITH GRAVE
        { 0x00FA, { 'u', 0, 0, 0 } },                  // LATIN SMALL LETTER U WITH ACUTE
        { 0x00FB, { 'u', 0, 0, 0 } },                  // LATIN SMALL LETTER U WITH CIRCUMFLEX
        { 0x00FC, { 'u', 0, 0, 0 } },                  // LATIN SMALL LETTER U WITH DIAERESIS
        { 0x00FD, { 'y', 0, 0, 0 } },                  // LATIN SMALL LETTER Y WITH ACUTE
        { 0x00FE, { 't', 'h', 0, 0 } },                // LATIN SMALL LETTER THORN
        { 0x00FF, { 'y', 0, 0, 0 } },                  // LATIN SMALL LETTER Y WITH DIAERESIS
        { 0x0100, { 'A', 0, 0, 0 } },                  // LATIN CAPITAL LETTER A WITH MACRON
        { 0x0101, { 'a', 0, 0, 0 } },                  // LATIN SMALL LETTER A WITH MACRON
        { 0x0102, { 'A', 0, 0, 0 } },                  // LATIN CAPITAL LETTER A WITH BREVE
        { 0x0103, { 'a', 0, 0, 0 } },                  // LATIN SMALL LETTER A WITH BREVE
        { 0x0104, { 'A', 0, 0, 0 } },                  // LATIN CAPITAL LETTER A WITH OGONEK
        { 0x0105, { 'a', 0, 0, 0 } },                  // LATIN SMALL LETTER A WITH OGONEK
        { 0x0106, { 'C', 0, 0, 0 } },                  // LATIN CAPITAL LETTER C WITH ACUTE
        { 0x0107, { 'c', 0, 0, 0 } },                  // LATIN SMALL LETTER C WITH ACUTE
        { 0x0108, { 'C', 0, 0, 0 } },                  // LATIN CAPITAL LETTER C WITH CIRCUMFLEX
        { 0x0109, { 'c', 0, 0, 0 } },                  // LATIN SMALL LETTER C WITH CIRCUMFLEX
        { 0x010A, { 'C', 0, 0, 0 } },                  // LATIN CAPITAL LETTER C WITH DOT ABOVE
        { 0x010B, { 'c', 0, 0, 0 } },                  // LATIN SMALL LETTER C WITH DOT ABOVE
        { 0x010C, { 'C', 0, 0, 0 } },                  // LATIN CAPITAL LETTER C WITH CARON
        { 0x010D, { 'c', 0, 0, 0 } },                  // LATIN SMALL LETTER C WITH CARON
        { 0x010E, { 'D', 0, 0, 0 } },                  // LATIN CAPITAL LETTER D WITH CARON
        { 0x010F, { 'd', 0, 0, 0 } },                  // LATIN SMALL LETTER D WITH CARON
        { 0x0110, { 'D', 0, 0, 0 } },                  // LATIN CAPITAL LETTER D WITH STROKE
        { 0x0111, { 'd', 0, 0, 0 } },                  // LATIN SMALL LETTER D WITH STROKE
        { 0x0112, { 'E', 0, 0, 0 } },                  // LATIN CAPITAL LETTER E WITH MACRON
        { 0x0113, { 'e', 0, 0, 0 } },                  // LATIN SMALL LETTER E WITH MACRON
        { 0x0114, { 'E', 0, 0, 0 } },                  // LATIN CAPITAL LETTER E WITH BREVE
        { 0x0115, { 'e', 0, 0, 0 } },                  // LATIN SMALL LETTER E WITH BREVE
        { 0x0116, { 'E', 0, 0, 0 } },                  // LATIN CAPITAL LETTER E WITH DOT ABOVE
        { 0x0117, { 'e', 0, 0, 0 } },                  // LATIN SMALL LETTER E WITH DOT ABOVE
        { 0x0118, { 'E', 0, 0, 0 } },                  // LATIN CAPITAL LETTER E WITH OGONEK
        { 0x0119, { 'e', 0, 0, 0 } },                  // LATIN SMALL LETTER E WITH OGONEK
        { 0x011A, { 'E', 0, 0, 0 } },                  // LATIN CAPITAL LETTER E WITH CARON
        { 0x011B, { 'e', 0, 0, 0 } },                  // LATIN SMALL LETTER E WITH CARON
        { 0x011C, { 'G', 0, 0, 0 } },                  // LATIN CAPITAL LETTER G WITH CIRCUMFLEX
        { 0x011D, { 'g', 0, 0, 0 } },                  // LATIN SMALL LETTER G WITH CIRCUMFLEX
        { 0x011E, { 'G', 0, 0, 0 } },                  // LATIN CAPITAL LETTER G WITH BREVE
        { 0x011F, { 'g', 0, 0, 0 } },                  // LATIN SMALL LETTER G WITH BREVE
        { 0x0120, { 'G', 0, 0, 0 } },                  // LATIN CAPITAL LETTER G WITH DOT ABOVE
        { 0x0121, { 'g', 0, 0, 0 } },                  // LATIN SMALL LETTER G WITH DOT ABOVE
        { 0x0122, { 'G', 0, 0, 0 } },                  // LATIN CAPITAL LETTER G WITH CEDILLA
        { 0x0123, { 'g', 0, 0, 0 } },                  // LATIN SMALL LETTER G WITH CEDILLA
        { 0x0124, { 'H', 0, 0, 0 } },                  // LATIN CAPITAL LETTER H WITH CIRCUMFLEX
        { 0x0125, { 'h', 0, 0, 0 } },                  // LATIN SMALL LETTER H WITH CIRCUMFLEX
        { 0x0126, { 'H', 0, 0, 0 } },                  // LATIN CAPITAL LETTER H WITH STROKE
        { 0x0127, { 'h', 0, 0, 0 } },                  // LATIN SMALL LETTER H WITH STROKE
        { 0x0128, { 'I', 0, 0, 0 } },                  // LATIN CAPITAL LETTER I WITH TILDE
        { 0x0129, { 'i', 0, 0, 0 } },                  // LATIN SMALL LETTER I WITH TILDE
        { 0x012A, { 'I', 0, 0, 0 } },                  // LATIN CAPITAL LETTER I WITH MACRON
        { 0x012B, { 'i', 0, 0, 0 } },                  // LATIN SMALL LETTER I WITH MACRON
        { 0x012C, { 'I', 0, 0, 0 } },                  // LATIN CAPITAL LETTER I WITH BREVE
        { 0x012D, { 'i', 0, 0, 0 } },                  // LATIN SMALL LETTER I WITH BREVE
        { 0x012E, { 'I', 0, 0, 0 } },                  // LATIN CAPITAL LETTER I WITH OGONEK
        { 0x012F, { 'i', 0, 0, 0 } },                  // LATIN SMALL LETTER I WITH OGONEK
        { 0x0130, { 'I', 0, 0, 0 } },                  // LATIN CAPITAL LETTER I WITH DOT ABOVE
        { 0x0131, { 'i', 0, 0, 0 } },                  // LATIN SMALL LETTER DOTLESS I
        { 0x0132, { 'I', 'J', 0, 0 } },                // LATIN CAPITAL LIGATURE IJ
        { 0x0133, { 'i', 'j', 0, 0 } },                // LATIN SMALL LIGATURE IJ
        { 0x0134, { 'J', 0, 0, 0 } },                  // LATIN CAPITAL LETTER J WITH CIRCUMFLEX
        { 0x0135, { 'j', 0, 0, 0 } },                  // LATIN SMALL LETTER J WITH CIRCUMFLEX
        { 0x0136, { 'K', 0, 0, 0 } },                  // LATIN CAPITAL LETTER K WITH CEDILLA
        { 0x0137, { 'k', 0, 0, 0 } },                  // LATIN SMALL LETTER K WITH CEDILLA
        { 0x0138, { 'k', 0, 0, 0 } },                  // LATIN SMALL LETTER KRA
        { 0x0139, { 'L', 0, 0, 0 } },                  // LATIN CAPITAL LETTER L WITH ACUTE
        { 0x013A, { 'l', 0, 0, 0 } },                  // LATIN SMALL LETTER L WITH ACUTE
        { 0x013B, { 'L', 0, 0, 0 } },                  // LATIN CAPITAL LETTER L WITH CEDILLA
        { 0x013C, { 'l', 0, 0, 0 } },                  // LATIN SMALL LETTER L WITH CEDILLA
        { 0x013D, { 'L', 0, 0, 0 } },                  // LATIN CAPITAL LETTER L WITH CARON
        { 0x013E, { 'l', 0, 0, 0 } },                  // LATIN SMALL LETTER L WITH CARON
        { 0x013F, { 'L', 0, 0, 0 } },                  // LATIN CAPITAL LETTER L WITH MIDDLE DOT
        { 0x0140, { 'l', 0, 0, 0 } },                  // LATIN SMALL LETTER L WITH MIDDLE DOT
        { 0x0141, { 'L', 0, 0, 0 } },                  // LATIN CAPITAL LETTER L WITH STROKE
        { 0x0142, { 'l', 0, 0, 0 } },                  // LATIN SMALL LETTER L WITH STROKE
        { 0x0143, { 'N', 0, 0, 0 } },                  // LATIN CAPITAL LETTER N WITH ACUTE
        { 0x0144, { 'n', 0, 0, 0 } },                  // LATIN SMALL LETTER N WITH ACUTE
        { 0x0145, { 'N', 0, 0, 0 } },                  // LATIN CAPITAL LETTER N WITH CEDILLA
        { 0x0146, { 'n', 0, 0, 0 } },                  // LATIN SMALL LETTER N WITH CEDILLA
        { 0x0147, { 'N', 0, 0, 0 } },                  // LATIN CAPITAL LETTER N WITH CARON
        { 0x0148, { 'n', 0, 0, 0 } },                  // LATIN SMALL LETTER N WITH CARON
        { 0x0149, { 39, 'n', 0, 0 } },                 // LATIN SMALL LETTER N PRECEDED BY APOSTROPHE
        { 0x014A, { 'n', 'g', 0, 0 } },                // LATIN CAPITAL LETTER ENG
        { 0x014B, { 'N', 'G', 0, 0 } },                // LATIN SMALL LETTER ENG
        { 0x014C, { 'O', 0, 0, 0 } },                  // LATIN CAPITAL LETTER O WITH MACRON
        { 0x014D, { 'o', 0, 0, 0 } },                  // LATIN SMALL LETTER O WITH MACRON
        { 0x014E, { 'O', 0, 0, 0 } },                  // LATIN CAPITAL LETTER O WITH BREVE
        { 0x014F, { 'o', 0, 0, 0 } },                  // LATIN SMALL LETTER O WITH BREVE
        { 0x0150, { 'O', 0, 0, 0 } },                  // LATIN CAPITAL LETTER O WITH DOUBLE ACUTE
        { 0x0151, { 'o', 0, 0, 0 } },                  // LATIN SMALL LETTER O WITH DOUBLE ACUTE
        { 0x0152, { 'O', 'E', 0, 0 } },                // LATIN CAPITAL LIGATURE OE
        { 0x0153, { 'o', 'e', 0, 0 } },                // LATIN SMALL LIGATURE OE
        { 0x0154, { 'R', 0, 0, 0 } },                  // LATIN CAPITAL LETTER R WITH ACUTE
        { 0x0155, { 'r', 0, 0, 0 } },                  // LATIN SMALL LETTER R WITH ACUTE
        { 0x0156, { 'R', 0, 0, 0 } },                  // LATIN CAPITAL LETTER R WITH CEDILLA
        { 0x0157, { 'r', 0, 0, 0 } },                  // LATIN SMALL LETTER R WITH CEDILLA
        { 0x0158, { 'R', 0, 0, 0 } },                  // LATIN CAPITAL LETTER R WITH CARON
        { 0x0159, { 'r', 0, 0, 0 } },                  // LATIN SMALL LETTER R WITH CARON
        { 0x015A, { 'S', 0, 0, 0 } },                  // LATIN CAPITAL LETTER S WITH ACUTE
        { 0x015B, { 's', 0, 0, 0 } },                  // LATIN SMALL LETTER S WITH ACUTE
        { 0x015C, { 'S', 0, 0, 0 } },                  // LATIN CAPITAL LETTER S WITH CIRCUMFLEX
        { 0x015D, { 's', 0, 0, 0 } },                  // LATIN SMALL LETTER S WITH CIRCUMFLEX
        { 0x015E, { 'S', 0, 0, 0 } },                  // LATIN CAPITAL LETTER S WITH CEDILLA
        { 0x015F, { 's', 0, 0, 0 } },                  // LATIN SMALL LETTER S WITH CEDILLA
        { 0x0160, { 'S', 0, 0, 0 } },                  // LATIN CAPITAL LETTER S WITH CARON
        { 0x0161, { 's', 0, 0, 0 } },                  // LATIN SMALL LETTER S WITH CARON
        { 0x0162, { 'T', 0, 0, 0 } },                  // LATIN CAPITAL LETTER T WITH CEDILLA
        { 0x0163, { 't', 0, 0, 0 } },                  // LATIN SMALL LETTER T WITH CEDILLA
        { 0x0164, { 'T', 0, 0, 0 } },                  // LATIN CAPITAL LETTER T WITH CARON
        { 0x0165, { 't', 0, 0, 0 } },                  // LATIN SMALL LETTER T WITH CARON
        { 0x0166, { 'T', 0, 0, 0 } },                  // LATIN CAPITAL LETTER T WITH STROKE
        { 0x0167, { 't', 0, 0, 0 } },                  // LATIN SMALL LETTER T WITH STROKE
        { 0x0168, { 'U', 0, 0, 0 } },                  // LATIN CAPITAL LETTER U WITH TILDE
        { 0x0169, { 'u', 0, 0, 0 } },                  // LATIN SMALL LETTER U WITH TILDE
        { 0x016A, { 'U', 0, 0, 0 } },                  // LATIN CAPITAL LETTER U WITH MACRON
        { 0x016B, { 'u', 0, 0, 0 } },                  // LATIN SMALL LETTER U WITH MACRON
        { 0x016C, { 'U', 0, 0, 0 } },                  // LATIN CAPITAL LETTER U WITH BREVE
        { 0x016D, { 'u', 0, 0, 0 } },                  // LATIN SMALL LETTER U WITH BREVE
        { 0x016E, { 'U', 0, 0, 0 } },                  // LATIN CAPITAL LETTER U WITH RING ABOVE
        { 0x016F, { 'u', 0, 0, 0 } },                  // LATIN SMALL LETTER U WITH RING ABOVE
        { 0x0170, { 'U', 0, 0, 0 } },                  // LATIN CAPITAL LETTER U WITH DOUBLE ACUTE
        { 0x0171, { 'u', 0, 0, 0 } },                  // LATIN SMALL LETTER U WITH DOUBLE ACUTE
        { 0x0172, { 'U', 0, 0, 0 } },                  // LATIN CAPITAL LETTER U WITH OGONEK
        { 0x0173, { 'u', 0, 0, 0 } },                  // LATIN SMALL LETTER U WITH OGONEK
        { 0x0174, { 'W', 0, 0, 0 } },                  // LATIN CAPITAL LETTER W WITH CIRCUMFLEX
        { 0x0175, { 'w', 0, 0, 0 } },                  // LATIN SMALL LETTER W WITH CIRCUMFLEX
        { 0x0176, { 'Y', 0, 0, 0 } },                  // LATIN CAPITAL LETTER Y WITH CIRCUMFLEX
        { 0x0177, { 'y', 0, 0, 0 } },                  // LATIN SMALL LETTER Y WITH CIRCUMFLEX
        { 0x0178, { 'Y', 0, 0, 0 } },                  // LATIN CAPITAL LETTER Y WITH DIAERESIS
        { 0x0179, { 'Z', 0, 0, 0 } },                  // LATIN CAPITAL LETTER Z WITH ACUTE
        { 0x017A, { 'z', 0, 0, 0 } },                  // LATIN SMALL LETTER Z WITH ACUTE
        { 0x017B, { 'Z', 0, 0, 0 } },                  // LATIN CAPITAL LETTER Z WITH DOT ABOVE
        { 0x017C, { 'z', 0, 0, 0 } },                  // LATIN SMALL LETTER Z WITH DOT ABOVE
        { 0x017D, { 'Z', 0, 0, 0 } },                  // LATIN CAPITAL LETTER Z WITH CARON
        { 0x017E, { 'z', 0, 0, 0 } },                  // LATIN SMALL LETTER Z WITH CARON
        { 0x017F, { 's', 0, 0, 0 } },                  // LATIN SMALL LETTER LONG S
        { 0x0180, { 'b', 0, 0, 0 } },                  // LATIN SMALL LETTER B WITH STROKE
        { 0x0181, { 'B', 0, 0, 0 } },                  // LATIN CAPITAL LETTER B WITH HOOK
        { 0x0182, { 'B', 0, 0, 0 } },                  // LATIN CAPITAL LETTER B WITH TOPBAR
        { 0x0183, { 'b', 0, 0, 0 } },                  // LATIN SMALL LETTER B WITH TOPBAR
        { 0x0184, { '6', 0, 0, 0 } },                  // LATIN CAPITAL LETTER TONE SIX
        { 0x0185, { '6', 0, 0, 0 } },                  // LATIN SMALL LETTER TONE SIX
        { 0x0186, { 'O', 0, 0, 0 } },                  // LATIN CAPITAL LETTER OPEN O
        { 0x0187, { 'C', 0, 0, 0 } },                  // LATIN CAPITAL LETTER C WITH HOOK
        { 0x0188, { 'c', 0, 0, 0 } },                  // LATIN SMALL LETTER C WITH HOOK
        { 0x0189, { 'D', 0, 0, 0 } },                  // LATIN CAPITAL LETTER AFRICAN D
        { 0x018A, { 'D', 0, 0, 0 } },                  // LATIN CAPITAL LETTER D WITH HOOK
        { 0x018B, { 'D', 0, 0, 0 } },                  // LATIN CAPITAL LETTER D WITH TOPBAR
        { 0x018C, { 'd', 0, 0, 0 } },                  // LATIN SMALL LETTER D WITH TOPBAR
        { 0x018D, { 'd', 0, 0, 0 } },                  // LATIN SMALL LETTER TURNED DELTA
        { 0x018E, { '3', 0, 0, 0 } },                  // LATIN CAPITAL LETTER REVERSED E
        { 0x018F, { '@', 0, 0, 0 } },                  // LATIN CAPITAL LETTER SCHWA
        { 0x0190, { 'E', 0, 0, 0 } },                  // LATIN CAPITAL LETTER OPEN E
        { 0x0191, { 'F', 0, 0, 0 } },                  // LATIN CAPITAL LETTER F WITH HOOK
        { 0x0192, { 'f', 0, 0, 0 } },                  // LATIN SMALL LETTER F WITH HOOK
        { 0x0193, { 'G', 0, 0, 0 } },                  // LATIN CAPITAL LETTER G WITH HOOK
        { 0x0194, { 'G', 0, 0, 0 } },                  // LATIN CAPITAL LETTER GAMMA
        { 0x0195, { 'h', 'v', 0, 0 } },                // LATIN SMALL LETTER HV
        { 0x0196, { 'I', 0, 0, 0 } },                  // LATIN CAPITAL LETTER IOTA
        { 0x0197, { 'I', 0, 0, 0 } },                  // LATIN CAPITAL LETTER I WITH STROKE
        { 0x0198, { 'K', 0, 0, 0 } },                  // LATIN CAPITAL LETTER K WITH HOOK
        { 0x0199, { 'k', 0, 0, 0 } },                  // LATIN SMALL LETTER K WITH HOOK
        { 0x019A, { 'l', 0, 0, 0 } },                  // LATIN SMALL LETTER L WITH BAR
        { 0x019B, { 'l', 0, 0, 0 } },                  // LATIN SMALL LETTER LAMBDA WITH STROKE
        { 0x019C, { 'W', 0, 0, 0 } },                  // LATIN CAPITAL LETTER TURNED M
        { 0x019D, { 'N', 0, 0, 0 } },                  // LATIN CAPITAL LETTER N WITH LEFT HOOK
        { 0x019E, { 'n', 0, 0, 0 } },                  // LATIN SMALL LETTER N WITH LONG RIGHT LEG
        { 0x019F, { 'O', 0, 0, 0 } },                  // LATIN CAPITAL LETTER O WITH MIDDLE TILDE
        { 0x01A0, { 'O', 0, 0, 0 } },                  // LATIN CAPITAL LETTER O WITH HORN
        { 0x01A1, { 'o', 0, 0, 0 } },                  // LATIN SMALL LETTER O WITH HORN
        { 0x01A2, { 'O', 'I', 0, 0 } },                // LATIN CAPITAL LETTER OI
        { 0x01A3, { 'o', 'i', 0, 0 } },                // LATIN SMALL LETTER OI
        { 0x01A4, { 'P', 0, 0, 0 } },                  // LATIN CAPITAL LETTER P WITH HOOK
        { 0x01A5, { 'p', 0, 0, 0 } },                  // LATIN SMALL LETTER P WITH HOOK
        { 0x01A6, { 'Y', 'R', 0, 0 } },                // LATIN LETTER YR
        { 0x01A7, { '2', 0, 0, 0 } },                  // LATIN CAPITAL LETTER TONE TWO
        { 0x01A8, { '2', 0, 0, 0 } },                  // LATIN SMALL LETTER TONE TWO
        { 0x01A9, { 'S', 'H', 0, 0 } },                // LATIN CAPITAL LETTER ESH
        { 0x01AA, { 's', 'h', 0, 0 } },                // LATIN LETTER REVERSED ESH LOOP
        { 0x01AB, { 't', 0, 0, 0 } },                  // LATIN SMALL LETTER T WITH PALATAL HOOK
        { 0x01AC, { 'T', 0, 0, 0 } },                  // LATIN CAPITAL LETTER T WITH HOOK
        { 0x01AD, { 't', 0, 0, 0 } },                  // LATIN SMALL LETTER T WITH HOOK
        { 0x01AE, { 'T', 0, 0, 0 } },                  // LATIN CAPITAL LETTER T WITH RETROFLEX HOOK
        { 0x01AF, { 'U', 0, 0, 0 } },                  // LATIN CAPITAL LETTER U WITH HORN
        { 0x01B0, { 'u', 0, 0, 0 } },                  // LATIN SMALL LETTER U WITH HORN
        { 0x01B1, { 'Y', 0, 0, 0 } },                  // LATIN CAPITAL LETTER UPSILON
        { 0x01B2, { 'V', 0, 0, 0 } },                  // LATIN CAPITAL LETTER V WITH HOOK
        { 0x01B3, { 'Y', 0, 0, 0 } },                  // LATIN CAPITAL LETTER Y WITH HOOK
        { 0x01B4, { 'y', 0, 0, 0 } },                  // LATIN SMALL LETTER Y WITH HOOK
        { 0x01B5, { 'Z', 0, 0, 0 } },                  // LATIN CAPITAL LETTER Z WITH STROKE
        { 0x01B6, { 'z', 0, 0, 0 } },                  // LATIN SMALL LETTER Z WITH STROKE
        { 0x01B7, { 'Z', 'H', 0, 0 } },                // LATIN CAPITAL LETTER EZH
        { 0x01B8, { 'Z', 'H', 0, 0 } },                // LATIN CAPITAL LETTER EZH REVERSED
        { 0x01B9, { 'z', 'h', 0, 0 } },                // LATIN SMALL LETTER EZH REVERSED
        { 0x01BA, { 'z', 'h', 0, 0 } },                // LATIN SMALL LETTER EZH WITH TAIL
        { 0x01BB, { '2', 0, 0, 0 } },                  // LATIN LETTER TWO WITH STROKE
        { 0x01BC, { '5', 0, 0, 0 } },                  // LATIN CAPITAL LETTER TONE FIVE
        { 0x01BD, { '5', 0, 0, 0 } },                  // LATIN SMALL LETTER TONE FIVE
        { 0x01BE, { 't', 's', 0, 0 } },                // LATIN LETTER INVERTED GLOTTAL STOP WITH STROKE
        { 0x01BF, { 'w', 0, 0, 0 } },                  // LATIN LETTER WYNN
        { 0x01C0, { '|', 0, 0, 0 } },                  // LATIN LETTER DENTAL CLICK
        { 0x01C1, { '|', '|', 0, 0 } },                // LATIN LETTER LATERAL CLICK
        { 0x01C2, { '|', '=', 0, 0 } },                // LATIN LETTER ALVEOLAR CLICK
        { 0x01C3, { '!', 0, 0, 0 } },                  // LATIN LETTER RETROFLEX CLICK
        { 0x01C4, { 'D', 'Z', 0, 0 } },                // LATIN CAPITAL LETTER DZ WITH CARON
        { 0x01C5, { 'D', 'z', 0, 0 } },                // LATIN CAPITAL LETTER D WITH SMALL LETTER Z WITH CARON
        { 0x01C6, { 'd', 'z', 0, 0 } },                // LATIN SMALL LETTER DZ WITH CARON
        { 0x01C7, { 'L', 'J', 0, 0 } },                // LATIN CAPITAL LETTER LJ
        { 0x01C8, { 'L', 'j', 0, 0 } },                // LATIN CAPITAL LETTER L WITH SMALL LETTER J
        { 0x01C9, { 'l', 'j', 0, 0 } },                // LATIN SMALL LETTER LJ
        { 0x01CA, { 'N', 'J', 0, 0 } },                // LATIN CAPITAL LETTER NJ
        { 0x01CB, { 'N', 'j', 0, 0 } },                // LATIN CAPITAL LETTER N WITH SMALL LETTER J
        { 0x01CC, { 'n', 'j', 0, 0 } },                // LATIN SMALL LETTER NJ
        { 0x01CD, { 'A', 0, 0, 0 } },                  // LATIN CAPITAL LETTER A WITH CARON
        { 0x01CE, { 'a', 0, 0, 0 } },                  // LATIN SMALL LETTER A WITH CARON
        { 0x01CF, { 'I', 0, 0, 0 } },                  // LATIN CAPITAL LETTER I WITH CARON
        { 0x01D0, { 'i', 0, 0, 0 } },                  // LATIN SMALL LETTER I WITH CARON
        { 0x01D1, { 'O', 0, 0, 0 } },                  // LATIN CAPITAL LETTER O WITH CARON
        { 0x01D2, { 'o', 0, 0, 0 } },                  // LATIN SMALL LETTER O WITH CARON
        { 0x01D3, { 'U', 0, 0, 0 } },                  // LATIN CAPITAL LETTER U WITH CARON
        { 0x01D4, { 'u', 0, 0, 0 } },                  // LATIN SMALL LETTER U WITH CARON
        { 0x01D5, { 'U', 0, 0, 0 } },                  // LATIN CAPITAL LETTER U WITH DIAERESIS AND MACRON
        { 0x01D6, { 'u', 0, 0, 0 } },                  // LATIN SMALL LETTER U WITH DIAERESIS AND MACRON
        { 0x01D7, { 'U', 0, 0, 0 } },                  // LATIN CAPITAL LETTER U WITH DIAERESIS AND ACUTE
        { 0x01D8, { 'u', 0, 0, 0 } },                  // LATIN SMALL LETTER U WITH DIAERESIS AND ACUTE
        { 0x01D9, { 'U', 0, 0, 0 } },                  // LATIN CAPITAL LETTER U WITH DIAERESIS AND CARON
        { 0x01DA, { 'u', 0, 0, 0 } },                  // LATIN SMALL LETTER U WITH DIAERESIS AND CARON
        { 0x01DB, { 'U', 0, 0, 0 } },                  // LATIN CAPITAL LETTER U WITH DIAERESIS AND GRAVE
        { 0x01DC, { 'u', 0, 0, 0 } },                  // LATIN SMALL LETTER U WITH DIAERESIS AND GRAVE
        { 0x01DD, { '@', 0, 0, 0 } },                  // LATIN SMALL LETTER TURNED E
        { 0x01DE, { 'A', 0, 0, 0 } },                  // LATIN CAPITAL LETTER A WITH DIAERESIS AND MACRON
        { 0x01DF, { 'a', 0, 0, 0 } },                  // LATIN SMALL LETTER A WITH DIAERESIS AND MACRON
        { 0x01E0, { 'A', 0, 0, 0 } },                  // LATIN CAPITAL LETTER A WITH DOT ABOVE AND MACRON
        { 0x01E1, { 'a', 0, 0, 0 } },                  // LATIN SMALL LETTER A WITH DOT ABOVE AND MACRON
        { 0x01E2, { 'A', 'E', 0, 0 } },                // LATIN CAPITAL LETTER AE WITH MACRON
        { 0x01E3, { 'a', 'e', 0, 0 } },                // LATIN SMALL LETTER AE WITH MACRON
        { 0x01E4, { 'G', 0, 0, 0 } },                  // LATIN CAPITAL LETTER G WITH STROKE
        { 0x01E5, { 'g', 0, 0, 0 } },                  // LATIN SMALL LETTER G WITH STROKE
        { 0x01E6, { 'G', 0, 0, 0 } },                  // LATIN CAPITAL LETTER G WITH CARON
        { 0x01E7, { 'g', 0, 0, 0 } },                  // LATIN SMALL LETTER G WITH CARON
        { 0x01E8, { 'K', 0, 0, 0 } },                  // LATIN CAPITAL LETTER K WITH CARON
        { 0x01E9, { 'k', 0, 0, 0 } },                  // LATIN SMALL LETTER K WITH CARON
        { 0x01EA, { 'O', 0, 0, 0 } },                  // LATIN CAPITAL LETTER O WITH OGONEK
        { 0x01EB, { 'o', 0, 0, 0 } },                  // LATIN SMALL LETTER O WITH OGONEK
        { 0x01EC, { 'O', 0, 0, 0 } },                  // LATIN CAPITAL LETTER O WITH OGONEK AND MACRON
        { 0x01ED, { 'o', 0, 0, 0 } },                  // LATIN SMALL LETTER O WITH OGONEK AND MACRON
        { 0x01EE, { 'Z', 'H', 0, 0 } },                // LATIN CAPITAL LETTER EZH WITH CARON
        { 0x01EF, { 'z', 'h', 0, 0 } },                // LATIN SMALL LETTER EZH WITH CARON
        { 0x01F0, { 'j', 0, 0, 0 } },                  // LATIN SMALL LETTER J WITH CARON
        { 0x01F1, { 'D', 'Z', 0, 0 } },                // LATIN CAPITAL LETTER DZ
        { 0x01F2, { 'D', 'z', 0, 0 } },                // LATIN CAPITAL LETTER D WITH SMALL LETTER Z
        { 0x01F3, { 'd', 'z', 0, 0 } },                // LATIN SMALL LETTER DZ
        { 0x01F4, { 'G', 0, 0, 0 } },                  // LATIN CAPITAL LETTER G WITH ACUTE
        { 0x01F5, { 'g', 0, 0, 0 } },                  // LATIN SMALL LETTER G WITH ACUTE
        { 0x01F6, { 'H', 'V', 0, 0 } },                // LATIN CAPITAL LETTER HWAIR
        { 0x01F7, { 'W', 0, 0, 0 } },                  // LATIN CAPITAL LETTER WYNN
        { 0x01F8, { 'N', 0, 0, 0 } },                  // LATIN CAPITAL LETTER N WITH GRAVE
        { 0x01F9, { 'n', 0, 0, 0 } },                  // LATIN SMALL LETTER N WITH GRAVE
        { 0x01FA, { 'A', 0, 0, 0 } },                  // LATIN CAPITAL LETTER A WITH RING ABOVE AND ACUTE
        { 0x01FB, { 'a', 0, 0, 0 } },                  // LATIN SMALL LETTER A WITH RING ABOVE AND ACUTE
        { 0x01FC, { 'A', 'E', 0, 0 } },                // LATIN CAPITAL LETTER AE WITH ACUTE
        { 0x01FD, { 'a', 'e', 0, 0 } },                // LATIN SMALL LETTER AE WITH ACUTE
        { 0x01FE, { 'O', 0, 0, 0 } },                  // LATIN CAPITAL LETTER O WITH STROKE AND ACUTE
        { 0x01FF, { 'o', 0, 0, 0 } },                  // LATIN SMALL LETTER O WITH STROKE AND ACUTE
        { 0x0200, { 'A', 0, 0, 0 } },                  // LATIN CAPITAL LETTER A WITH DOUBLE GRAVE
        { 0x0201, { 'a', 0, 0, 0 } },                  // LATIN SMALL LETTER A WITH DOUBLE GRAVE
        { 0x0202, { 'A', 0, 0, 0 } },                  // LATIN CAPITAL LETTER A WITH INVERTED BREVE
        { 0x0203, { 'a', 0, 0, 0 } },                  // LATIN SMALL LETTER A WITH INVERTED BREVE
        { 0x0204, { 'E', 0, 0, 0 } },                  // LATIN CAPITAL LETTER E WITH DOUBLE GRAVE
        { 0x0205, { 'e', 0, 0, 0 } },                  // LATIN SMALL LETTER E WITH DOUBLE GRAVE
        { 0x0206, { 'E', 0, 0, 0 } },                  // LATIN CAPITAL LETTER E WITH INVERTED BREVE
        { 0x0207, { 'e', 0, 0, 0 } },                  // LATIN SMALL LETTER E WITH INVERTED BREVE
        { 0x0208, { 'I', 0, 0, 0 } },                  // LATIN CAPITAL LETTER I WITH DOUBLE GRAVE
        { 0x0209, { 'i', 0, 0, 0 } },                  // LATIN SMALL LETTER I WITH DOUBLE GRAVE
        { 0x020A, { 'I', 0, 0, 0 } },                  // LATIN CAPITAL LETTER I WITH INVERTED BREVE
        { 0x020B, { 'i', 0, 0, 0 } },                  // LATIN SMALL LETTER I WITH INVERTED BREVE
        { 0x020C, { 'O', 0, 0, 0 } },                  // LATIN CAPITAL LETTER O WITH DOUBLE GRAVE
        { 0x020D, { 'o', 0, 0, 0 } },                  // LATIN SMALL LETTER O WITH DOUBLE GRAVE
        { 0x020E, { 'O', 0, 0, 0 } },                  // LATIN CAPITAL LETTER O WITH INVERTED BREVE
        { 0x020F, { 'o', 0, 0, 0 } },                  // LATIN SMALL LETTER O WITH INVERTED BREVE
        { 0x0210, { 'R', 0, 0, 0 } },                  // LATIN CAPITAL LETTER R WITH DOUBLE GRAVE
        { 0x0211, { 'r', 0, 0, 0 } },                  // LATIN SMALL LETTER R WITH DOUBLE GRAVE
        { 0x0212, { 'R', 0, 0, 0 } },                  // LATIN CAPITAL LETTER R WITH INVERTED BREVE
        { 0x0213, { 'r', 0, 0, 0 } },                  // LATIN SMALL LETTER R WITH INVERTED BREVE
        { 0x0214, { 'U', 0, 0, 0 } },                  // LATIN CAPITAL LETTER U WITH DOUBLE GRAVE
        { 0x0215, { 'u', 0, 0, 0 } },                  // LATIN SMALL LETTER U WITH DOUBLE GRAVE
        { 0x0216, { 'U', 0, 0, 0 } },                  // LATIN CAPITAL LETTER U WITH INVERTED BREVE
        { 0x0217, { 'u', 0, 0, 0 } },                  // LATIN SMALL LETTER U WITH INVERTED BREVE
        { 0x0218, { 'S', 0, 0, 0 } },                  // LATIN CAPITAL LETTER S WITH COMMA BELOW
        { 0x0219, { 's', 0, 0, 0 } },                  // LATIN SMALL LETTER S WITH COMMA BELOW
        { 0x021A, { 'T', 0, 0, 0 } },                  // LATIN CAPITAL LETTER T WITH COMMA BELOW
        { 0x021B, { 't', 0, 0, 0 } },                  // LATIN SMALL LETTER T WITH COMMA BELOW
        { 0x021C, { 'Y', 0, 0, 0 } },                  // LATIN CAPITAL LETTER YOGH
        { 0x021D, { 'y', 0, 0, 0 } },                  // LATIN SMALL LETTER YOGH
        { 0x021E, { 'H', 0, 0, 0 } },                  // LATIN CAPITAL LETTER H WITH CARON
        { 0x021F, { 'h', 0, 0, 0 } },                  // LATIN SMALL LETTER H WITH CARON
        { 0x0220, { 'N', 0, 0, 0 } },                  // LATIN CAPITAL LETTER N WITH LONG RIGHT LEG
        { 0x0221, { 'd', 0, 0, 0 } },                  // LATIN SMALL LETTER D WITH CURL
        { 0x0222, { 'O', 'U', 0, 0 } },                // LATIN CAPITAL LETTER OU
        { 0x0223, { 'o', 'u', 0, 0 } },                // LATIN SMALL LETTER OU
        { 0x0224, { 'Z', 0, 0, 0 } },                  // LATIN CAPITAL LETTER Z WITH HOOK
        { 0x0225, { 'z', 0, 0, 0 } },                  // LATIN SMALL LETTER Z WITH HOOK
        { 0x0226, { 'A', 0, 0, 0 } },                  // LATIN CAPITAL LETTER A WITH DOT ABOVE
        { 0x0227, { 'a', 0, 0, 0 } },                  // LATIN SMALL LETTER A WITH DOT ABOVE
        { 0x0228, { 'E', 0, 0, 0 } },                  // LATIN CAPITAL LETTER E WITH CEDILLA
        { 0x0229, { 'e', 0, 0, 0 } },                  // LATIN SMALL LETTER E WITH CEDILLA
        { 0x022A, { 'O', 0, 0, 0 } },                  // LATIN CAPITAL LETTER O WITH DIAERESIS AND MACRON
        { 0x022B, { 'o', 0, 0, 0 } },                  // LATIN SMALL LETTER O WITH DIAERESIS AND MACRON
        { 0x022C, { 'O', 0, 0, 0 } },                  // LATIN CAPITAL LETTER O WITH TILDE AND MACRON
        { 0x022D, { 'o', 0, 0, 0 } },                  // LATIN SMALL LETTER O WITH TILDE AND MACRON
        { 0x022E, { 'O', 0, 0, 0 } },                  // LATIN CAPITAL LETTER O WITH DOT ABOVE
        { 0x022F, { 'o', 0, 0, 0 } },                  // LATIN SMALL LETTER O WITH DOT ABOVE
        { 0x0230, { 'O', 0, 0, 0 } },                  // LATIN CAPITAL LETTER O WITH DOT ABOVE AND MACRON
        { 0x0231, { 'o', 0, 0, 0 } },                  // LATIN SMALL LETTER O WITH DOT ABOVE AND MACRON
        { 0x0232, { 'Y', 0, 0, 0 } },                  // LATIN CAPITAL LETTER Y WITH MACRON
        { 0x0233, { 'y', 0, 0, 0 } },                  // LATIN SMALL LETTER Y WITH MACRON
        { 0x0234, { 'l', 0, 0, 0 } },                  // LATIN SMALL LETTER L WITH CURL
        { 0x0235, { 'n', 0, 0, 0 } },                  // LATIN SMALL LETTER N WITH CURL
        { 0x0236, { 't', 0, 0, 0 } },                  // LATIN SMALL LETTER T WITH CURL
        { 0x0237, { 'j', 0, 0, 0 } },                  // LATIN SMALL LETTER DOTLESS J
        { 0x0238, { 'd', 'b', 0, 0 } },                // LATIN SMALL LETTER DB DIGRAPH
        { 0x0239, { 'q', 'p', 0, 0 } },                // LATIN SMALL LETTER QP DIGRAPH
        { 0x023A, { 'A', 0, 0, 0 } },                  // LATIN CAPITAL LETTER A WITH STROKE
        { 0x023B, { 'C', 0, 0, 0 } },                  // LATIN CAPITAL LETTER C WITH STROKE
        { 0x023C, { 'c', 0, 0, 0 } },                  // LATIN SMALL LETTER C WITH STROKE
        { 0x023D, { 'L', 0, 0, 0 } },                  // LATIN CAPITAL LETTER L WITH BAR
        { 0x023E, { 'T', 0, 0, 0 } },                  // LATIN CAPITAL LETTER T WITH DIAGONAL STROKE
        { 0x023F, { 's', 0, 0, 0 } },                  // LATIN SMALL LETTER S WITH SWASH TAIL
        { 0x0240, { 'z', 0, 0, 0 } },                  // LATIN SMALL LETTER Z WITH SWASH TAIL
        { 0x0243, { 'B', 0, 0, 0 } },                  // LATIN CAPITAL LETTER B WITH STROKE
        { 0x0244, { 'U', 0, 0, 0 } },                  // LATIN CAPITAL LETTER U BAR
        { 0x0245, { '^', 0, 0, 0 } },                  // LATIN CAPITAL LETTER TURNED V
        { 0x0246, { 'E', 0, 0, 0 } },                  // LATIN CAPITAL LETTER E WITH STROKE
        { 0x0247, { 'e', 0, 0, 0 } },                  // LATIN SMALL LETTER E WITH STROKE
        { 0x0248, { 'J', 0, 0, 0 } },                  // LATIN CAPITAL LETTER J WITH STROKE
        { 0x0249, { 'j', 0, 0, 0 } },                  // LATIN SMALL LETTER J WITH STROKE
        { 0x024A, { 'q', 0, 0, 0 } },                  // LATIN CAPITAL LETTER SMALL Q WITH HOOK TAIL
        { 0x024B, { 'q', 0, 0, 0 } },                  // LATIN SMALL LETTER Q WITH HOOK TAIL
        { 0x024C, { 'R', 0, 0, 0 } },                  // LATIN CAPITAL LETTER R WITH STROKE
        { 0x024D, { 'r', 0, 0, 0 } },                  // LATIN SMALL LETTER R WITH STROKE
        { 0x024E, { 'Y', 0, 0, 0 } },                  // LATIN CAPITAL LETTER Y WITH STROKE
        { 0x024F, { 'y', 0, 0, 0 } },                  // LATIN SMALL LETTER Y WITH STROKE
        { 0x0250, { 'a', 0, 0, 0 } },                  // LATIN SMALL LETTER TURNED A
        { 0x0251, { 'a', 0, 0, 0 } },                  // LATIN SMALL LETTER ALPHA
        { 0x0252, { 'a', 0, 0, 0 } },                  // LATIN SMALL LETTER TURNED ALPHA
        { 0x0253, { 'b', 0, 0, 0 } },                  // LATIN SMALL LETTER B WITH HOOK
        { 0x0254, { 'o', 0, 0, 0 } },                  // LATIN SMALL LETTER OPEN O
        { 0x0255, { 'c', 0, 0, 0 } },                  // LATIN SMALL LETTER C WITH CURL
        { 0x0256, { 'd', 0, 0, 0 } },                  // LATIN SMALL LETTER D WITH TAIL
        { 0x0257, { 'd', 0, 0, 0 } },                  // LATIN SMALL LETTER D WITH HOOK
        { 0x0258, { 'e', 0, 0, 0 } },                  // LATIN SMALL LETTER REVERSED E
        { 0x0259, { '@', 0, 0, 0 } },                  // LATIN SMALL LETTER SCHWA
        { 0x025A, { '@', 0, 0, 0 } },                  // LATIN SMALL LETTER SCHWA WITH HOOK
        { 0x025B, { 'e', 0, 0, 0 } },                  // LATIN SMALL LETTER OPEN E
        { 0x025C, { 'e', 0, 0, 0 } },                  // LATIN SMALL LETTER REVERSED OPEN E
        { 0x025D, { 'e', 0, 0, 0 } },                  // LATIN SMALL LETTER REVERSED OPEN E WITH HOOK
        { 0x025E, { 'e', 0, 0, 0 } },                  // LATIN SMALL LETTER CLOSED REVERSED OPEN E
        { 0x025F, { 'j', 0, 0, 0 } },                  // LATIN SMALL LETTER DOTLESS J WITH STROKE
        { 0x0260, { 'g', 0, 0, 0 } },                  // LATIN SMALL LETTER G WITH HOOK
        { 0x0261, { 'g', 0, 0, 0 } },                  // LATIN SMALL LETTER SCRIPT G
        { 0x0262, { 'g', 0, 0, 0 } },                  // LATIN LETTER SMALL CAPITAL G
        { 0x0263, { 'g', 0, 0, 0 } },                  // LATIN SMALL LETTER GAMMA
        { 0x0264, { 'u', 0, 0, 0 } },                  // LATIN SMALL LETTER RAMS HORN
        { 0x0265, { 'Y', 0, 0, 0 } },                  // LATIN SMALL LETTER TURNED H
        { 0x0266, { 'h', 0, 0, 0 } },                  // LATIN SMALL LETTER H WITH HOOK
        { 0x0267, { 'h', 0, 0, 0 } },                  // LATIN SMALL LETTER HENG WITH HOOK
        { 0x0268, { 'i', 0, 0, 0 } },                  // LATIN SMALL LETTER I WITH STROKE
        { 0x0269, { 'i', 0, 0, 0 } },                  // LATIN SMALL LETTER IOTA
        { 0x026A, { 'I', 0, 0, 0 } },                  // LATIN LETTER SMALL CAPITAL I
        { 0x026B, { 'l', 0, 0, 0 } },                  // LATIN SMALL LETTER L WITH MIDDLE TILDE
        { 0x026C, { 'l', 0, 0, 0 } },                  // LATIN SMALL LETTER L WITH BELT
        { 0x026D, { 'l', 0, 0, 0 } },                  // LATIN SMALL LETTER L WITH RETROFLEX HOOK
        { 0x026E, { 'l', 'Z', 0, 0 } },                // LATIN SMALL LETTER LEZH
        { 0x026F, { 'W', 0, 0, 0 } },                  // LATIN SMALL LETTER TURNED M
        { 0x0270, { 'W', 0, 0, 0 } },                  // LATIN SMALL LETTER TURNED M WITH LONG LEG
        { 0x0271, { 'm', 0, 0, 0 } },                  // LATIN SMALL LETTER M WITH HOOK
        { 0x0272, { 'n', 0, 0, 0 } },                  // LATIN SMALL LETTER N WITH LEFT HOOK
        { 0x0273, { 'n', 0, 0, 0 } },                  // LATIN SMALL LETTER N WITH RETROFLEX HOOK
        { 0x0274, { 'n', 0, 0, 0 } },                  // LATIN LETTER SMALL CAPITAL N
        { 0x0275, { 'o', 0, 0, 0 } },                  // LATIN SMALL LETTER BARRED O
        { 0x0276, { 'O', 'E', 0, 0 } },                // LATIN LETTER SMALL CAPITAL OE
        { 0x0277, { 'O', 0, 0, 0 } },                  // LATIN SMALL LETTER CLOSED OMEGA
        { 0x0278, { 'F', 0, 0, 0 } },                  // LATIN SMALL LETTER PHI
        { 0x0279, { 'r', 0, 0, 0 } },                  // LATIN SMALL LETTER TURNED R
        { 0x027A, { 'r', 0, 0, 0 } },                  // LATIN SMALL LETTER TURNED R WITH LONG LEG
        { 0x027B, { 'r', 0, 0, 0 } },                  // LATIN SMALL LETTER TURNED R WITH HOOK
        { 0x027C, { 'r', 0, 0, 0 } },                  // LATIN SMALL LETTER R WITH LONG LEG
        { 0x027D, { 'r', 0, 0, 0 } },                  // LATIN SMALL LETTER R WITH TAIL
        { 0x027E, { 'r', 0, 0, 0 } },                  // LATIN SMALL LETTER R WITH FISHHOOK
        { 0x027F, { 'r', 0, 0, 0 } },                  // LATIN SMALL LETTER REVERSED R WITH FISHHOOK
        { 0x0280, { 'R', 0, 0, 0 } },                  // LATIN LETTER SMALL CAPITAL R
        { 0x0281, { 'R', 0, 0, 0 } },                  // LATIN LETTER SMALL CAPITAL INVERTED R
        { 0x0282, { 's', 0, 0, 0 } },                  // LATIN SMALL LETTER S WITH HOOK
        { 0x0283, { 'S', 0, 0, 0 } },                  // LATIN SMALL LETTER ESH
        { 0x0284, { 'j', 0, 0, 0 } },                  // LATIN SMALL LETTER DOTLESS J WITH STROKE AND HOOK
        { 0x0285, { 'S', 0, 0, 0 } },                  // LATIN SMALL LETTER SQUAT REVERSED ESH
        { 0x0286, { 'S', 0, 0, 0 } },                  // LATIN SMALL LETTER ESH WITH CURL
        { 0x0287, { 't', 0, 0, 0 } },                  // LATIN SMALL LETTER TURNED T
        { 0x0288, { 't', 0, 0, 0 } },                  // LATIN SMALL LETTER T WITH RETROFLEX HOOK
        { 0x0289, { 'u', 0, 0, 0 } },                  // LATIN SMALL LETTER U BAR
        { 0x028A, { 'U', 0, 0, 0 } },                  // LATIN SMALL LETTER UPSILON
        { 0x028B, { 'v', 0, 0, 0 } },                  // LATIN SMALL LETTER V WITH HOOK
        { 0x028C, { '^', 0, 0, 0 } },                  // LATIN SMALL LETTER TURNED V
        { 0x028D, { 'w', 0, 0, 0 } },                  // LATIN SMALL LETTER TURNED W
        { 0x028E, { 'y', 0, 0, 0 } },                  // LATIN SMALL LETTER TURNED Y
        { 0x028F, { 'Y', 0, 0, 0 } },                  // LATIN LETTER SMALL CAPITAL Y
        { 0x0290, { 'z', 0, 0, 0 } },                  // LATIN SMALL LETTER Z WITH RETROFLEX HOOK
        { 0x0291, { 'z', 0, 0, 0 } },                  // LATIN SMALL LETTER Z WITH CURL
        { 0x0292, { 'Z', 0, 0, 0 } },                  // LATIN SMALL LETTER EZH
        { 0x0293, { 'Z', 0, 0, 0 } },                  // LATIN SMALL LETTER EZH WITH CURL
        { 0x0294, { '?', 0, 0, 0 } },                  // LATIN LETTER GLOTTAL STOP
        { 0x0295, { '?', 0, 0, 0 } },                  // LATIN LETTER PHARYNGEAL VOICED FRICATIVE
        { 0x0296, { '?', 0, 0, 0 } },                  // LATIN LETTER INVERTED GLOTTAL STOP
        { 0x0297, { 'C', 0, 0, 0 } },                  // LATIN LETTER STRETCHED C
        { 0x0298, { '@', 0, 0, 0 } },                  // LATIN LETTER BILABIAL CLICK
        { 0x0299, { 'B', 0, 0, 0 } },                  // LATIN LETTER SMALL CAPITAL B
        { 0x029A, { 'E', 0, 0, 0 } },                  // LATIN SMALL LETTER CLOSED OPEN E
        { 0x029B, { 'G', 0, 0, 0 } },                  // LATIN LETTER SMALL CAPITAL G WITH HOOK
        { 0x029C, { 'H', 0, 0, 0 } },                  // LATIN LETTER SMALL CAPITAL H
        { 0x029D, { 'j', 0, 0, 0 } },                  // LATIN SMALL LETTER J WITH CROSSED-TAIL
        { 0x029E, { 'k', 0, 0, 0 } },                  // LATIN SMALL LETTER TURNED K
        { 0x029F, { 'L', 0, 0, 0 } },                  // LATIN LETTER SMALL CAPITAL L
        { 0x02A0, { 'q', 0, 0, 0 } },                  // LATIN SMALL LETTER Q WITH HOOK
        { 0x02A1, { '?', 0, 0, 0 } },                  // LATIN LETTER GLOTTAL STOP WITH STROKE
        { 0x02A2, { '?', 0, 0, 0 } },                  // LATIN LETTER REVERSED GLOTTAL STOP WITH STROKE
        { 0x02A3, { 'd', 'z', 0, 0 } },                // LATIN SMALL LETTER DZ DIGRAPH
        { 0x02A4, { 'd', 'Z', 0, 0 } },                // LATIN SMALL LETTER DEZH DIGRAPH
        { 0x02A5, { 'd', 'z', 0, 0 } },                // LATIN SMALL LETTER DZ DIGRAPH WITH CURL
        { 0x02A6, { 't', 's', 0, 0 } },                // LATIN SMALL LETTER TS DIGRAPH
        { 0x02A7, { 't', 'S', 0, 0 } },                // LATIN SMALL LETTER TESH DIGRAPH
        { 0x02A8, { 't', 'C', 0, 0 } },                // LATIN SMALL LETTER TC DIGRAPH WITH CURL
        { 0x02A9, { 'f', 'N', 0, 0 } },                // LATIN SMALL LETTER FENG DIGRAPH
        { 0x02AA, { 'l', 's', 0, 0 } },                // LATIN SMALL LETTER LS DIGRAPH
        { 0x02AB, { 'l', 'z', 0, 0 } },                // LATIN SMALL LETTER LZ DIGRAPH
        { 0x02AC, { 'W', 'W', 0, 0 } },                // LATIN LETTER BILABIAL PERCUSSIVE
        { 0x02AD, { ']', ']', 0, 0 } },                // LATIN LETTER BIDENTAL PERCUSSIVE
        { 0x02AE, { 'h', 0, 0, 0 } },                  // LATIN SMALL LETTER TURNED H WITH FISHHOOK
        { 0x02AF, { 'h', 0, 0, 0 } },                  // LATIN SMALL LETTER TURNED H WITH FISHHOOK AND TAIL
        { 0x02B0, { 'k', 0, 0, 0 } },                  // MODIFIER LETTER SMALL H
        { 0x02B1, { 'h', 0, 0, 0 } },                  // MODIFIER LETTER SMALL H WITH HOOK
        { 0x02B2, { 'j', 0, 0, 0 } },                  // MODIFIER LETTER SMALL J
        { 0x02B3, { 'r', 0, 0, 0 } },                  // MODIFIER LETTER SMALL R
        { 0x02B4, { 'r', 0, 0, 0 } },                  // MODIFIER LETTER SMALL TURNED R
        { 0x02B5, { 'r', 0, 0, 0 } },                  // MODIFIER LETTER SMALL TURNED R WITH HOOK
        { 0x02B6, { 'r', 0, 0, 0 } },                  // MODIFIER LETTER SMALL CAPITAL INVERTED R
        { 0x02B7, { 'w', 0, 0, 0 } },                  // MODIFIER LETTER SMALL W
        { 0x02B8, { 'y', 0, 0, 0 } },                  // MODIFIER LETTER SMALL Y
        { 0x02B9, { 39, 0, 0, 0 } },                   // MODIFIER LETTER PRIME
        { 0x02BA, { 92, '"', 0, 0 } },                 // MODIFIER LETTER DOUBLE PRIME
        { 0x02BB, { '`', 0, 0, 0 } },                  // MODIFIER LETTER TURNED COMMA
        { 0x02BC, { 39, 0, 0, 0 } },                   // MODIFIER LETTER APOSTROPHE
        { 0x02BD, { '`', 0, 0, 0 } },                  // MODIFIER LETTER REVERSED COMMA
        { 0x02BE, { '`', 0, 0, 0 } },                  // MODIFIER LETTER RIGHT HALF RING
        { 0x02BF, { 39, 0, 0, 0 } },                   // MODIFIER LETTER LEFT HALF RING
        { 0x02C0, { '?', 0, 0, 0 } },                  // MODIFIER LETTER GLOTTAL STOP
        { 0x02C1, { '?', 0, 0, 0 } },                  // MODIFIER LETTER REVERSED GLOTTAL STOP
        { 0x02C6, { '^', 0, 0, 0 } },                  // MODIFIER LETTER CIRCUMFLEX ACCENT
        { 0x02C7, { 'V', 0, 0, 0 } },                  // CARON
        { 0x02C8, { 39, 0, 0, 0 } },                   // MODIFIER LETTER VERTICAL LINE
        { 0x02C9, { '-', 0, 0, 0 } },                  // MODIFIER LETTER MACRON
        { 0x02CA, { '/', 0, 0, 0 } },                  // MODIFIER LETTER ACUTE ACCENT
        { 0x02CB, { 92, 0, 0, 0 } },                   // MODIFIER LETTER GRAVE ACCENT
        { 0x02CC, { ',', 0, 0, 0 } },                  // MODIFIER LETTER LOW VERTICAL LINE
        { 0x02CD, { '_', 0, 0, 0 } },                  // MODIFIER LETTER LOW MACRON
        { 0x02CE, { 92, 0, 0, 0 } },                   // MODIFIER LETTER LOW GRAVE ACCENT
        { 0x02CF, { '/', 0, 0, 0 } },                  // MODIFIER LETTER LOW ACUTE ACCENT
        { 0x02D0, { ':', 0, 0, 0 } },                  // MODIFIER LETTER TRIANGULAR COLON
        { 0x02D1, { '.', 0, 0, 0 } },                  // MODIFIER LETTER HALF TRIANGULAR COLON
        { 0x02E0, { 'G', 0, 0, 0 } },                  // MODIFIER LETTER SMALL GAMMA
        { 0x02E1, { 'l', 0, 0, 0 } },                  // MODIFIER LETTER SMALL L
        { 0x02E2, { 's', 0, 0, 0 } },                  // MODIFIER LETTER SMALL S
        { 0x02E3, { 'x', 0, 0, 0 } },                  // MODIFIER LETTER SMALL X
        { 0x02E4, { '?', 0, 0, 0 } },                  // MODIFIER LETTER SMALL REVERSED GLOTTAL STOP
        { 0x02EC, { 'V', 0, 0, 0 } },                  // MODIFIER LETTER VOICING
        { 0x02EE, { 92, '"', 0, 0 } },                 // MODIFIER LETTER DOUBLE APOSTROPHE
        { 0x0374, { 39, 0, 0, 0 } },                   // GREEK NUMERAL SIGN
        { 0x0386, { 'A', 0, 0, 0 } },                  // GREEK CAPITAL LETTER ALPHA WITH TONOS
        { 0x0388, { 'E', 0, 0, 0 } },                  // GREEK CAPITAL LETTER EPSILON WITH TONOS
        { 0x0389, { 'E', 0, 0, 0 } },                  // GREEK CAPITAL LETTER ETA WITH TONOS
        { 0x038A, { 'I', 0, 0, 0 } },                  // GREEK CAPITAL LETTER IOTA WITH TONOS
        { 0x038C, { 'O', 0, 0, 0 } },                  // GREEK CAPITAL LETTER OMICRON WITH TONOS
        { 0x038E, { 'U', 0, 0, 0 } },                  // GREEK CAPITAL LETTER UPSILON WITH TONOS
        { 0x038F, { 'O', 0, 0, 0 } },                  // GREEK CAPITAL LETTER OMEGA WITH TONOS
        { 0x0390, { 'I', 0, 0, 0 } },                  // GREEK SMALL LETTER IOTA WITH DIALYTIKA AND TONOS
        { 0x0391, { 'A', 0, 0, 0 } },                  // GREEK CAPITAL LETTER ALPHA
        { 0x0392, { 'B', 0, 0, 0 } },                  // GREEK CAPITAL LETTER BETA
        { 0x0393, { 'G', 0, 0, 0 } },                  // GREEK CAPITAL LETTER GAMMA
        { 0x0394, { 'D', 0, 0, 0 } },                  // GREEK CAPITAL LETTER DELTA
        { 0x0395, { 'E', 0, 0, 0 } },                  // GREEK CAPITAL LETTER EPSILON
        { 0x0396, { 'Z', 0, 0, 0 } },                  // GREEK CAPITAL LETTER ZETA
        { 0x0397, { 'E', 0, 0, 0 } },                  // GREEK CAPITAL LETTER ETA
        { 0x0398, { 'T', 'h', 0, 0 } },                // GREEK CAPITAL LETTER THETA
        { 0x0399, { 'I', 0, 0, 0 } },                  // GREEK CAPITAL LETTER IOTA
        { 0x039A, { 'K', 0, 0, 0 } },                  // GREEK CAPITAL LETTER KAPPA
        { 0x039B, { 'L', 0, 0, 0 } },                  // GREEK CAPITAL LETTER LAMDA
        { 0x039C, { 'M', 0, 0, 0 } },                  // GREEK CAPITAL LETTER MU
        { 0x039D, { 'N', 0, 0, 0 } },                  // GREEK CAPITAL LETTER NU
        { 0x039E, { 'K', 's', 0, 0 } },                // GREEK CAPITAL LETTER XI
        { 0x039F, { 'O', 0, 0, 0 } },                  // GREEK CAPITAL LETTER OMICRON
        { 0x03A0, { 'P', 0, 0, 0 } },                  // GREEK CAPITAL LETTER PI
        { 0x03A1, { 'R', 0, 0, 0 } },                  // GREEK CAPITAL LETTER RHO
        { 0x03A3, { 'S', 0, 0, 0 } },                  // GREEK CAPITAL LETTER SIGMA
        { 0x03A4, { 'T', 0, 0, 0 } },                  // GREEK CAPITAL LETTER TAU
        { 0x03A5, { 'U', 0, 0, 0 } },                  // GREEK CAPITAL LETTER UPSILON
        { 0x03A6, { 'P', 'h', 0, 0 } },                // GREEK CAPITAL LETTER PHI
        { 0x03A7, { 'K', 'h', 0, 0 } },                // GREEK CAPITAL LETTER CHI
        { 0x03A8, { 'P', 's', 0, 0 } },                // GREEK CAPITAL LETTER PSI
        { 0x03A9, { 'O', 0, 0, 0 } },                  // GREEK CAPITAL LETTER OMEGA
        { 0x03AA, { 'I', 0, 0, 0 } },                  // GREEK CAPITAL LETTER IOTA WITH DIALYTIKA
        { 0x03AB, { 'U', 0, 0, 0 } },                  // GREEK CAPITAL LETTER UPSILON WITH DIALYTIKA
        { 0x03AC, { 'a', 0, 0, 0 } },                  // GREEK SMALL LETTER ALPHA WITH TONOS
        { 0x03AD, { 'e', 0, 0, 0 } },                  // GREEK SMALL LETTER EPSILON WITH TONOS
        { 0x03AE, { 'e', 0, 0, 0 } },                  // GREEK SMALL LETTER ETA WITH TONOS
        { 0x03AF, { 'i', 0, 0, 0 } },                  // GREEK SMALL LETTER IOTA WITH TONOS
        { 0x03B0, { 'u', 0, 0, 0 } },                  // GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND TONOS
        { 0x03B1, { 'a', 0, 0, 0 } },                  // GREEK SMALL LETTER ALPHA
        { 0x03B2, { 'b', 0, 0, 0 } },                  // GREEK SMALL LETTER BETA
        { 0x03B3, { 'g', 0, 0, 0 } },                  // GREEK SMALL LETTER GAMMA
        { 0x03B4, { 'd', 0, 0, 0 } },                  // GREEK SMALL LETTER DELTA
        { 0x03B5, { 'e', 0, 0, 0 } },                  // GREEK SMALL LETTER EPSILON
        { 0x03B6, { 'z', 0, 0, 0 } },                  // GREEK SMALL LETTER ZETA
        { 0x03B7, { 'e', 0, 0, 0 } },                  // GREEK SMALL LETTER ETA
        { 0x03B8, { 't', 'h', 0, 0 } },                // GREEK SMALL LETTER THETA
        { 0x03B9, { 'i', 0, 0, 0 } },                  // GREEK SMALL LETTER IOTA
        { 0x03BA, { 'k', 0, 0, 0 } },                  // GREEK SMALL LETTER KAPPA
        { 0x03BB, { 'l', 0, 0, 0 } },                  // GREEK SMALL LETTER LAMDA
        { 0x03BC, { 'm', 0, 0, 0 } },                  // GREEK SMALL LETTER MU
        { 0x03BD, { 'n', 0, 0, 0 } },                  // GREEK SMALL LETTER NU
        { 0x03BE, { 'x', 0, 0, 0 } },                  // GREEK SMALL LETTER XI
        { 0x03BF, { 'o', 0, 0, 0 } },                  // GREEK SMALL LETTER OMICRON
        { 0x03C0, { 'p', 0, 0, 0 } },                  // GREEK SMALL LETTER PI
        { 0x03C1, { 'r', 0, 0, 0 } },                  // GREEK SMALL LETTER RHO
        { 0x03C2, { 's', 0, 0, 0 } },                  // GREEK SMALL LETTER FINAL SIGMA
        { 0x03C3, { 's', 0, 0, 0 } },                  // GREEK SMALL LETTER SIGMA
        { 0x03C4, { 't', 0, 0, 0 } },                  // GREEK SMALL LETTER TAU
        { 0x03C5, { 'u', 0, 0, 0 } },                  // GREEK SMALL LETTER UPSILON
        { 0x03C6, { 'p', 'h', 0, 0 } },                // GREEK SMALL LETTER PHI
        { 0x03C7, { 'k', 'h', 0, 0 } },                // GREEK SMALL LETTER CHI
        { 0x03C8, { 'p', 's', 0, 0 } },                // GREEK SMALL LETTER PSI
        { 0x03C9, { 'o', 0, 0, 0 } },                  // GREEK SMALL LETTER OMEGA
        { 0x03CA, { 'i', 0, 0, 0 } },                  // GREEK SMALL LETTER IOTA WITH DIALYTIKA
        { 0x03CB, { 'u', 0, 0, 0 } },                  // GREEK SMALL LETTER UPSILON WITH DIALYTIKA
        { 0x03CC, { 'o', 0, 0, 0 } },                  // GREEK SMALL LETTER OMICRON WITH TONOS
        { 0x03CD, { 'u', 0, 0, 0 } },                  // GREEK SMALL LETTER UPSILON WITH TONOS
        { 0x03CE, { 'o', 0, 0, 0 } },                  // GREEK SMALL LETTER OMEGA WITH TONOS
        { 0x03D0, { 'b', 0, 0, 0 } },                  // GREEK BETA SYMBOL
        { 0x03D1, { 't', 'h', 0, 0 } },                // GREEK THETA SYMBOL
        { 0x03D2, { 'U', 0, 0, 0 } },                  // GREEK UPSILON WITH HOOK SYMBOL
        { 0x03D3, { 'U', 0, 0, 0 } },                  // GREEK UPSILON WITH ACUTE AND HOOK SYMBOL
        { 0x03D4, { 'U', 0, 0, 0 } },                  // GREEK UPSILON WITH DIAERESIS AND HOOK SYMBOL
        { 0x03D5, { 'p', 'h', 0, 0 } },                // GREEK PHI SYMBOL
        { 0x03D6, { 'p', 0, 0, 0 } },                  // GREEK PI SYMBOL
        { 0x03D7, { '&', 0, 0, 0 } },                  // GREEK KAI SYMBOL
        { 0x03DA, { 'S', 't', 0, 0 } },                // GREEK LETTER STIGMA
        { 0x03DB, { 's', 't', 0, 0 } },                // GREEK SMALL LETTER STIGMA
        { 0x03DC, { 'W', 0, 0, 0 } },                  // GREEK LETTER DIGAMMA
        { 0x03DD, { 'w', 0, 0, 0 } },                  // GREEK SMALL LETTER DIGAMMA
        { 0x03DE, { 'Q', 0, 0, 0 } },                  // GREEK LETTER KOPPA
        { 0x03DF, { 'q', 0, 0, 0 } },                  // GREEK SMALL LETTER KOPPA
        { 0x03E0, { 'S', 'p', 0, 0 } },                // GREEK LETTER SAMPI
        { 0x03E1, { 's', 'p', 0, 0 } },                // GREEK SMALL LETTER SAMPI
        { 0x03E2, { 'S', 'h', 0, 0 } },                // COPTIC CAPITAL LETTER SHEI
        { 0x03E3, { 's', 'h', 0, 0 } },                // COPTIC SMALL LETTER SHEI
        { 0x03E4, { 'F', 0, 0, 0 } },                  // COPTIC CAPITAL LETTER FEI
        { 0x03E5, { 'f', 0, 0, 0 } },                  // COPTIC SMALL LETTER FEI
        { 0x03E6, { 'K', 'h', 0, 0 } },                // COPTIC CAPITAL LETTER KHEI
        { 0x03E7, { 'k', 'h', 0, 0 } },                // COPTIC SMALL LETTER KHEI
        { 0x03E8, { 'H', 0, 0, 0 } },                  // COPTIC CAPITAL LETTER HORI
        { 0x03E9, { 'h', 0, 0, 0 } },                  // COPTIC SMALL LETTER HORI
        { 0x03EA, { 'G', 0, 0, 0 } },                  // COPTIC CAPITAL LETTER GANGIA
        { 0x03EB, { 'g', 0, 0, 0 } },                  // COPTIC SMALL LETTER GANGIA
        { 0x03EC, { 'C', 'H', 0, 0 } },                // COPTIC CAPITAL LETTER SHIMA
        { 0x03ED, { 'c', 'h', 0, 0 } },                // COPTIC SMALL LETTER SHIMA
        { 0x03EE, { 'T', 'i', 0, 0 } },                // COPTIC CAPITAL LETTER DEI
        { 0x03EF, { 't', 'i', 0, 0 } },                // COPTIC SMALL LETTER DEI
        { 0x03F0, { 'k', 0, 0, 0 } },                  // GREEK KAPPA SYMBOL
        { 0x03F1, { 'r', 0, 0, 0 } },                  // GREEK RHO SYMBOL
        { 0x03F2, { 'c', 0, 0, 0 } },                  // GREEK LUNATE SIGMA SYMBOL
        { 0x03F3, { 'j', 0, 0, 0 } },                  // GREEK LETTER YOT
        { 0x0400, { 'I', 'e', 0, 0 } },                // CYRILLIC CAPITAL LETTER IE WITH GRAVE
        { 0x0401, { 'I', 'o', 0, 0 } },                // CYRILLIC CAPITAL LETTER IO
        { 0x0402, { 'D', 'j', 0, 0 } },                // CYRILLIC CAPITAL LETTER DJE
        { 0x0403, { 'G', 'j', 0, 0 } },                // CYRILLIC CAPITAL LETTER GJE
        { 0x0404, { 'I', 'e', 0, 0 } },                // CYRILLIC CAPITAL LETTER UKRAINIAN IE
        { 0x0405, { 'D', 'z', 0, 0 } },                // CYRILLIC CAPITAL LETTER DZE
        { 0x0406, { 'I', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER BYELORUSSIAN-UKRAINIAN I
        { 0x0407, { 'Y', 'i', 0, 0 } },                // CYRILLIC CAPITAL LETTER YI
        { 0x0408, { 'J', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER JE
        { 0x0409, { 'L', 'j', 0, 0 } },                // CYRILLIC CAPITAL LETTER LJE
        { 0x040A, { 'N', 'j', 0, 0 } },                // CYRILLIC CAPITAL LETTER NJE
        { 0x040B, { 'T', 's', 'h', 0 } },              // CYRILLIC CAPITAL LETTER TSHE
        { 0x040C, { 'K', 'j', 0, 0 } },                // CYRILLIC CAPITAL LETTER KJE
        { 0x040D, { 'I', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER I WITH GRAVE
        { 0x040E, { 'U', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER SHORT U
        { 0x040F, { 'D', 'z', 'h', 0 } },              // CYRILLIC CAPITAL LETTER DZHE
        { 0x0410, { 'A', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER A
        { 0x0411, { 'B', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER BE
        { 0x0412, { 'V', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER VE
        { 0x0413, { 'G', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER GHE
        { 0x0414, { 'D', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER DE
        { 0x0415, { 'E', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER IE
        { 0x0416, { 'Z', 'h', 0, 0 } },                // CYRILLIC CAPITAL LETTER ZHE
        { 0x0417, { 'Z', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER ZE
        { 0x0418, { 'I', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER I
        { 0x0419, { 'I', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER SHORT I
        { 0x041A, { 'K', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER KA
        { 0x041B, { 'L', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER EL
        { 0x041C, { 'M', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER EM
        { 0x041D, { 'N', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER EN
        { 0x041E, { 'O', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER O
        { 0x041F, { 'P', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER PE
        { 0x0420, { 'R', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER ER
        { 0x0421, { 'S', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER ES
        { 0x0422, { 'T', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER TE
        { 0x0423, { 'U', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER U
        { 0x0424, { 'F', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER EF
        { 0x0425, { 'K', 'h', 0, 0 } },                // CYRILLIC CAPITAL LETTER HA
        { 0x0426, { 'T', 's', 0, 0 } },                // CYRILLIC CAPITAL LETTER TSE
        { 0x0427, { 'C', 'h', 0, 0 } },                // CYRILLIC CAPITAL LETTER CHE
        { 0x0428, { 'S', 'h', 0, 0 } },                // CYRILLIC CAPITAL LETTER SHA
        { 0x0429, { 'S', 'h', 'c', 'h' } },            // CYRILLIC CAPITAL LETTER SHCHA
        { 0x042A, { 39, 0, 0, 0 } },                   // CYRILLIC CAPITAL LETTER HARD SIGN
        { 0x042B, { 'Y', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER YERU
        { 0x042C, { 39, 0, 0, 0 } },                   // CYRILLIC CAPITAL LETTER SOFT SIGN
        { 0x042D, { 'E', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER E
        { 0x042E, { 'I', 'u', 0, 0 } },                // CYRILLIC CAPITAL LETTER YU
        { 0x042F, { 'I', 'a', 0, 0 } },                // CYRILLIC CAPITAL LETTER YA
        { 0x0430, { 'a', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER A
        { 0x0431, { 'b', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER BE
        { 0x0432, { 'v', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER VE
        { 0x0433, { 'g', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER GHE
        { 0x0434, { 'd', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER DE
        { 0x0435, { 'e', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER IE
        { 0x0436, { 'z', 'h', 0, 0 } },                // CYRILLIC SMALL LETTER ZHE
        { 0x0437, { 'z', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER ZE
        { 0x0438, { 'i', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER I
        { 0x0439, { 'i', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER SHORT I
        { 0x043A, { 'k', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER KA
        { 0x043B, { 'l', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER EL
        { 0x043C, { 'm', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER EM
        { 0x043D, { 'n', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER EN
        { 0x043E, { 'o', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER O
        { 0x043F, { 'p', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER PE
        { 0x0440, { 'r', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER ER
        { 0x0441, { 's', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER ES
        { 0x0442, { 't', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER TE
        { 0x0443, { 'u', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER U
        { 0x0444, { 'f', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER EF
        { 0x0445, { 'k', 'h', 0, 0 } },                // CYRILLIC SMALL LETTER HA
        { 0x0446, { 't', 's', 0, 0 } },                // CYRILLIC SMALL LETTER TSE
        { 0x0447, { 'c', 'h', 0, 0 } },                // CYRILLIC SMALL LETTER CHE
        { 0x0448, { 's', 'h', 0, 0 } },                // CYRILLIC SMALL LETTER SHA
        { 0x0449, { 's', 'h', 'c', 'h' } },            // CYRILLIC SMALL LETTER SHCHA
        { 0x044A, { 39, 0, 0, 0 } },                   // CYRILLIC SMALL LETTER HARD SIGN
        { 0x044B, { 'y', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER YERU
        { 0x044C, { 39, 0, 0, 0 } },                   // CYRILLIC SMALL LETTER SOFT SIGN
        { 0x044D, { 'e', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER E
        { 0x044E, { 'i', 'u', 0, 0 } },                // CYRILLIC SMALL LETTER YU
        { 0x044F, { 'i', 'a', 0, 0 } },                // CYRILLIC SMALL LETTER YA
        { 0x0450, { 'i', 'e', 0, 0 } },                // CYRILLIC SMALL LETTER IE WITH GRAVE
        { 0x0451, { 'i', 'o', 0, 0 } },                // CYRILLIC SMALL LETTER IO
        { 0x0452, { 'd', 'j', 0, 0 } },                // CYRILLIC SMALL LETTER DJE
        { 0x0453, { 'g', 'j', 0, 0 } },                // CYRILLIC SMALL LETTER GJE
        { 0x0454, { 'i', 'e', 0, 0 } },                // CYRILLIC SMALL LETTER UKRAINIAN IE
        { 0x0455, { 'd', 'z', 0, 0 } },                // CYRILLIC SMALL LETTER DZE
        { 0x0456, { 'i', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER BYELORUSSIAN-UKRAINIAN I
        { 0x0457, { 'y', 'i', 0, 0 } },                // CYRILLIC SMALL LETTER YI
        { 0x0458, { 'j', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER JE
        { 0x0459, { 'l', 'j', 0, 0 } },                // CYRILLIC SMALL LETTER LJE
        { 0x045A, { 'n', 'j', 0, 0 } },                // CYRILLIC SMALL LETTER NJE
        { 0x045B, { 't', 's', 'h', 0 } },              // CYRILLIC SMALL LETTER TSHE
        { 0x045C, { 'k', 'j', 0, 0 } },                // CYRILLIC SMALL LETTER KJE
        { 0x045D, { 'i', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER I WITH GRAVE
        { 0x045E, { 'u', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER SHORT U
        { 0x045F, { 'd', 'z', 'h', 0 } },              // CYRILLIC SMALL LETTER DZHE
        { 0x0460, { 'O', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER OMEGA
        { 0x0461, { 'o', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER OMEGA
        { 0x0462, { 'E', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER YAT
        { 0x0463, { 'e', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER YAT
        { 0x0464, { 'I', 'e', 0, 0 } },                // CYRILLIC CAPITAL LETTER IOTIFIED E
        { 0x0465, { 'i', 'e', 0, 0 } },                // CYRILLIC SMALL LETTER IOTIFIED E
        { 0x0466, { 'E', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER LITTLE YUS
        { 0x0467, { 'e', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER LITTLE YUS
        { 0x0468, { 'I', 'e', 0, 0 } },                // CYRILLIC CAPITAL LETTER IOTIFIED LITTLE YUS
        { 0x0469, { 'i', 'e', 0, 0 } },                // CYRILLIC SMALL LETTER IOTIFIED LITTLE YUS
        { 0x046A, { 'O', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER BIG YUS
        { 0x046B, { 'o', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER BIG YUS
        { 0x046C, { 'I', 'o', 0, 0 } },                // CYRILLIC CAPITAL LETTER IOTIFIED BIG YUS
        { 0x046D, { 'i', 'o', 0, 0 } },                // CYRILLIC SMALL LETTER IOTIFIED BIG YUS
        { 0x046E, { 'K', 's', 0, 0 } },                // CYRILLIC CAPITAL LETTER KSI
        { 0x046F, { 'k', 's', 0, 0 } },                // CYRILLIC SMALL LETTER KSI
        { 0x0470, { 'P', 's', 0, 0 } },                // CYRILLIC CAPITAL LETTER PSI
        { 0x0471, { 'p', 's', 0, 0 } },                // CYRILLIC SMALL LETTER PSI
        { 0x0472, { 'F', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER FITA
        { 0x0473, { 'f', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER FITA
        { 0x0474, { 'Y', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER IZHITSA
        { 0x0475, { 'y', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER IZHITSA
        { 0x0476, { 'Y', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER IZHITSA WITH DOUBLE GRAVE ACCENT
        { 0x0477, { 'y', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER IZHITSA WITH DOUBLE GRAVE ACCENT
        { 0x0478, { 'u', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER UK
        { 0x0479, { 'u', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER UK
        { 0x047A, { 'O', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER ROUND OMEGA
        { 0x047B, { 'o', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER ROUND OMEGA
        { 0x047C, { 'O', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER OMEGA WITH TITLO
        { 0x047D, { 'o', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER OMEGA WITH TITLO
        { 0x047E, { 'O', 't', 0, 0 } },                // CYRILLIC CAPITAL LETTER OT
        { 0x047F, { 'o', 't', 0, 0 } },                // CYRILLIC SMALL LETTER OT
        { 0x0480, { 'Q', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER KOPPA
        { 0x0481, { 'q', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER KOPPA
        { 0x048C, { 92, '"', 0, 0 } },                 // CYRILLIC CAPITAL LETTER SEMISOFT SIGN
        { 0x048D, { 92, '"', 0, 0 } },                 // CYRILLIC SMALL LETTER SEMISOFT SIGN
        { 0x048E, { 'R', 39, 0, 0 } },                 // CYRILLIC CAPITAL LETTER ER WITH TICK
        { 0x048F, { 'r', 39, 0, 0 } },                 // CYRILLIC SMALL LETTER ER WITH TICK
        { 0x0490, { 'G', 39, 0, 0 } },                 // CYRILLIC CAPITAL LETTER GHE WITH UPTURN
        { 0x0491, { 'g', 39, 0, 0 } },                 // CYRILLIC SMALL LETTER GHE WITH UPTURN
        { 0x0492, { 'G', 39, 0, 0 } },                 // CYRILLIC CAPITAL LETTER GHE WITH STROKE
        { 0x0493, { 'g', 39, 0, 0 } },                 // CYRILLIC SMALL LETTER GHE WITH STROKE
        { 0x0494, { 'G', 39, 0, 0 } },                 // CYRILLIC CAPITAL LETTER GHE WITH MIDDLE HOOK
        { 0x0495, { 'g', 39, 0, 0 } },                 // CYRILLIC SMALL LETTER GHE WITH MIDDLE HOOK
        { 0x0496, { 'Z', 'h', 39, 0 } },               // CYRILLIC CAPITAL LETTER ZHE WITH DESCENDER
        { 0x0497, { 'z', 'h', 39, 0 } },               // CYRILLIC SMALL LETTER ZHE WITH DESCENDER
        { 0x0498, { 'Z', 39, 0, 0 } },                 // CYRILLIC CAPITAL LETTER ZE WITH DESCENDER
        { 0x0499, { 'z', 39, 0, 0 } },                 // CYRILLIC SMALL LETTER ZE WITH DESCENDER
        { 0x049A, { 'K', 39, 0, 0 } },                 // CYRILLIC CAPITAL LETTER KA WITH DESCENDER
        { 0x049B, { 'k', 39, 0, 0 } },                 // CYRILLIC SMALL LETTER KA WITH DESCENDER
        { 0x049C, { 'K', 39, 0, 0 } },                 // CYRILLIC CAPITAL LETTER KA WITH VERTICAL STROKE
        { 0x049D, { 'k', 39, 0, 0 } },                 // CYRILLIC SMALL LETTER KA WITH VERTICAL STROKE
        { 0x049E, { 'K', 39, 0, 0 } },                 // CYRILLIC CAPITAL LETTER KA WITH STROKE
        { 0x049F, { 'k', 39, 0, 0 } },                 // CYRILLIC SMALL LETTER KA WITH STROKE
        { 0x04A0, { 'K', 39, 0, 0 } },                 // CYRILLIC CAPITAL LETTER BASHKIR KA
        { 0x04A1, { 'k', 39, 0, 0 } },                 // CYRILLIC SMALL LETTER BASHKIR KA
        { 0x04A2, { 'N', 39, 0, 0 } },                 // CYRILLIC CAPITAL LETTER EN WITH DESCENDER
        { 0x04A3, { 'n', 39, 0, 0 } },                 // CYRILLIC SMALL LETTER EN WITH DESCENDER
        { 0x04A4, { 'N', 'g', 0, 0 } },                // CYRILLIC CAPITAL LIGATURE EN GHE
        { 0x04A5, { 'n', 'g', 0, 0 } },                // CYRILLIC SMALL LIGATURE EN GHE
        { 0x04A6, { 'P', 39, 0, 0 } },                 // CYRILLIC CAPITAL LETTER PE WITH MIDDLE HOOK
        { 0x04A7, { 'p', 39, 0, 0 } },                 // CYRILLIC SMALL LETTER PE WITH MIDDLE HOOK
        { 0x04A8, { 'K', 'h', 0, 0 } },                // CYRILLIC CAPITAL LETTER ABKHASIAN HA
        { 0x04A9, { 'k', 'h', 0, 0 } },                // CYRILLIC SMALL LETTER ABKHASIAN HA
        { 0x04AA, { 'S', 39, 0, 0 } },                 // CYRILLIC CAPITAL LETTER ES WITH DESCENDER
        { 0x04AB, { 's', 39, 0, 0 } },                 // CYRILLIC SMALL LETTER ES WITH DESCENDER
        { 0x04AC, { 'T', 39, 0, 0 } },                 // CYRILLIC CAPITAL LETTER TE WITH DESCENDER
        { 0x04AD, { 't', 39, 0, 0 } },                 // CYRILLIC SMALL LETTER TE WITH DESCENDER
        { 0x04AE, { 'U', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER STRAIGHT U
        { 0x04AF, { 'u', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER STRAIGHT U
        { 0x04B0, { 'U', 39, 0, 0 } },                 // CYRILLIC CAPITAL LETTER STRAIGHT U WITH STROKE
        { 0x04B1, { 'u', 39, 0, 0 } },                 // CYRILLIC SMALL LETTER STRAIGHT U WITH STROKE
        { 0x04B2, { 'K', 'h', 39, 0 } },               // CYRILLIC CAPITAL LETTER HA WITH DESCENDER
        { 0x04B3, { 'k', 'h', 39, 0 } },               // CYRILLIC SMALL LETTER HA WITH DESCENDER
        { 0x04B4, { 'T', 't', 's', 0 } },              // CYRILLIC CAPITAL LIGATURE TE TSE
        { 0x04B5, { 't', 't', 's', 0 } },              // CYRILLIC SMALL LIGATURE TE TSE
        { 0x04B6, { 'C', 'h', 39, 0 } },               // CYRILLIC CAPITAL LETTER CHE WITH DESCENDER
        { 0x04B7, { 'c', 'h', 39, 0 } },               // CYRILLIC SMALL LETTER CHE WITH DESCENDER
        { 0x04B8, { 'C', 'h', 39, 0 } },               // CYRILLIC CAPITAL LETTER CHE WITH VERTICAL STROKE
        { 0x04B9, { 'c', 'h', 39, 0 } },               // CYRILLIC SMALL LETTER CHE WITH VERTICAL STROKE
        { 0x04BA, { 'H', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER SHHA
        { 0x04BB, { 'h', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER SHHA
        { 0x04BC, { 'C', 'h', 0, 0 } },                // CYRILLIC CAPITAL LETTER ABKHASIAN CHE
        { 0x04BD, { 'c', 'h', 0, 0 } },                // CYRILLIC SMALL LETTER ABKHASIAN CHE
        { 0x04BE, { 'C', 'h', 39, 0 } },               // CYRILLIC CAPITAL LETTER ABKHASIAN CHE WITH DESCENDER
        { 0x04BF, { 'c', 'h', 39, 0 } },               // CYRILLIC SMALL LETTER ABKHASIAN CHE WITH DESCENDER
        { 0x04C0, { '`', 0, 0, 0 } },                  // CYRILLIC LETTER PALOCHKA
        { 0x04C1, { 'Z', 'h', 0, 0 } },                // CYRILLIC CAPITAL LETTER ZHE WITH BREVE
        { 0x04C2, { 'z', 'h', 0, 0 } },                // CYRILLIC SMALL LETTER ZHE WITH BREVE
        { 0x04C3, { 'K', 39, 0, 0 } },                 // CYRILLIC CAPITAL LETTER KA WITH HOOK
        { 0x04C4, { 'k', 39, 0, 0 } },                 // CYRILLIC SMALL LETTER KA WITH HOOK
        { 0x04C7, { 'N', 39, 0, 0 } },                 // CYRILLIC CAPITAL LETTER EN WITH HOOK
        { 0x04C8, { 'n', 39, 0, 0 } },                 // CYRILLIC SMALL LETTER EN WITH HOOK
        { 0x04CB, { 'C', 'h', 0, 0 } },                // CYRILLIC CAPITAL LETTER KHAKASSIAN CHE
        { 0x04CC, { 'c', 'h', 0, 0 } },                // CYRILLIC SMALL LETTER KHAKASSIAN CHE
        { 0x04D0, { 'a', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER A WITH BREVE
        { 0x04D1, { 'a', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER A WITH BREVE
        { 0x04D2, { 'A', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER A WITH DIAERESIS
        { 0x04D3, { 'a', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER A WITH DIAERESIS
        { 0x04D4, { 'A', 'e', 0, 0 } },                // CYRILLIC CAPITAL LIGATURE A IE
        { 0x04D5, { 'a', 'e', 0, 0 } },                // CYRILLIC SMALL LIGATURE A IE
        { 0x04D6, { 'I', 'e', 0, 0 } },                // CYRILLIC CAPITAL LETTER IE WITH BREVE
        { 0x04D7, { 'i', 'e', 0, 0 } },                // CYRILLIC SMALL LETTER IE WITH BREVE
        { 0x04D8, { '@', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER SCHWA
        { 0x04D9, { '@', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER SCHWA
        { 0x04DA, { '@', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER SCHWA WITH DIAERESIS
        { 0x04DB, { '@', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER SCHWA WITH DIAERESIS
        { 0x04DC, { 'Z', 'h', 0, 0 } },                // CYRILLIC CAPITAL LETTER ZHE WITH DIAERESIS
        { 0x04DD, { 'z', 'h', 0, 0 } },                // CYRILLIC SMALL LETTER ZHE WITH DIAERESIS
        { 0x04DE, { 'Z', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER ZE WITH DIAERESIS
        { 0x04DF, { 'z', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER ZE WITH DIAERESIS
        { 0x04E0, { 'D', 'z', 0, 0 } },                // CYRILLIC CAPITAL LETTER ABKHASIAN DZE
        { 0x04E1, { 'd', 'z', 0, 0 } },                // CYRILLIC SMALL LETTER ABKHASIAN DZE
        { 0x04E2, { 'I', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER I WITH MACRON
        { 0x04E3, { 'i', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER I WITH MACRON
        { 0x04E4, { 'I', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER I WITH DIAERESIS
        { 0x04E5, { 'i', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER I WITH DIAERESIS
        { 0x04E6, { 'O', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER O WITH DIAERESIS
        { 0x04E7, { 'o', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER O WITH DIAERESIS
        { 0x04E8, { 'O', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER BARRED O
        { 0x04E9, { 'o', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER BARRED O
        { 0x04EA, { 'O', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER BARRED O WITH DIAERESIS
        { 0x04EB, { 'o', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER BARRED O WITH DIAERESIS
        { 0x04EC, { 'E', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER E WITH DIAERESIS
        { 0x04ED, { 'e', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER E WITH DIAERESIS
        { 0x04EE, { 'U', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER U WITH MACRON
        { 0x04EF, { 'u', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER U WITH MACRON
        { 0x04F0, { 'U', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER U WITH DIAERESIS
        { 0x04F1, { 'u', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER U WITH DIAERESIS
        { 0x04F2, { 'U', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER U WITH DOUBLE ACUTE
        { 0x04F3, { 'u', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER U WITH DOUBLE ACUTE
        { 0x04F4, { 'C', 'h', 0, 0 } },                // CYRILLIC CAPITAL LETTER CHE WITH DIAERESIS
        { 0x04F5, { 'c', 'h', 0, 0 } },                // CYRILLIC SMALL LETTER CHE WITH DIAERESIS
        { 0x04F8, { 'Y', 0, 0, 0 } },                  // CYRILLIC CAPITAL LETTER YERU WITH DIAERESIS
        { 0x04F9, { 'y', 0, 0, 0 } },                  // CYRILLIC SMALL LETTER YERU WITH DIAERESIS
        { 0x0531, { 'A', 0, 0, 0 } },                  // ARMENIAN CAPITAL LETTER AYB
        { 0x0532, { 'B', 0, 0, 0 } },                  // ARMENIAN CAPITAL LETTER BEN
        { 0x0533, { 'G', 0, 0, 0 } },                  // ARMENIAN CAPITAL LETTER GIM
        { 0x0534, { 'D', 0, 0, 0 } },                  // ARMENIAN CAPITAL LETTER DA
        { 0x0535, { 'E', 0, 0, 0 } },                  // ARMENIAN CAPITAL LETTER ECH
        { 0x0536, { 'Z', 0, 0, 0 } },                  // ARMENIAN CAPITAL LETTER ZA
        { 0x0537, { 'E', 0, 0, 0 } },                  // ARMENIAN CAPITAL LETTER EH
        { 0x0538, { 'E', 0, 0, 0 } },                  // ARMENIAN CAPITAL LETTER ET
        { 0x0539, { 'T', '`', 0, 0 } },                // ARMENIAN CAPITAL LETTER TO
        { 0x053A, { 'Z', 'h', 0, 0 } },                // ARMENIAN CAPITAL LETTER ZHE
        { 0x053B, { 'I', 0, 0, 0 } },                  // ARMENIAN CAPITAL LETTER INI
        { 0x053C, { 'L', 0, 0, 0 } },                  // ARMENIAN CAPITAL LETTER LIWN
        { 0x053D, { 'K', 'h', 0, 0 } },                // ARMENIAN CAPITAL LETTER XEH
        { 0x053E, { 'T', 's', 0, 0 } },                // ARMENIAN CAPITAL LETTER CA
        { 0x053F, { 'K', 0, 0, 0 } },                  // ARMENIAN CAPITAL LETTER KEN
        { 0x0540, { 'H', 0, 0, 0 } },                  // ARMENIAN CAPITAL LETTER HO
        { 0x0541, { 'D', 'z', 0, 0 } },                // ARMENIAN CAPITAL LETTER JA
        { 0x0542, { 'G', 'h', 0, 0 } },                // ARMENIAN CAPITAL LETTER GHAD
        { 0x0543, { 'C', 'h', 0, 0 } },                // ARMENIAN CAPITAL LETTER CHEH
        { 0x0544, { 'M', 0, 0, 0 } },                  // ARMENIAN CAPITAL LETTER MEN
        { 0x0545, { 'Y', 0, 0, 0 } },                  // ARMENIAN CAPITAL LETTER YI
        { 0x0546, { 'N', 0, 0, 0 } },                  // ARMENIAN CAPITAL LETTER NOW
        { 0x0547, { 'S', 'h', 0, 0 } },                // ARMENIAN CAPITAL LETTER SHA
        { 0x0548, { 'O', 0, 0, 0 } },                  // ARMENIAN CAPITAL LETTER VO
        { 0x0549, { 'C', 'h', '`', 0 } },              // ARMENIAN CAPITAL LETTER CHA
        { 0x054A, { 'P', 0, 0, 0 } },                  // ARMENIAN CAPITAL LETTER PEH
        { 0x054B, { 'J', 0, 0, 0 } },                  // ARMENIAN CAPITAL LETTER JHEH
        { 0x054C, { 'R', 'h', 0, 0 } },                // ARMENIAN CAPITAL LETTER RA
        { 0x054D, { 'S', 0, 0, 0 } },                  // ARMENIAN CAPITAL LETTER SEH
        { 0x054E, { 'V', 0, 0, 0 } },                  // ARMENIAN CAPITAL LETTER VEW
        { 0x054F, { 'T', 0, 0, 0 } },                  // ARMENIAN CAPITAL LETTER TIWN
        { 0x0550, { 'R', 0, 0, 0 } },                  // ARMENIAN CAPITAL LETTER REH
        { 0x0551, { 'T', 's', '`', 0 } },              // ARMENIAN CAPITAL LETTER CO
        { 0x0552, { 'W', 0, 0, 0 } },                  // ARMENIAN CAPITAL LETTER YIWN
        { 0x0553, { 'P', '`', 0, 0 } },                // ARMENIAN CAPITAL LETTER PIWR
        { 0x0554, { 'K', '`', 0, 0 } },                // ARMENIAN CAPITAL LETTER KEH
        { 0x0555, { 'O', 0, 0, 0 } },                  // ARMENIAN CAPITAL LETTER OH
        { 0x0556, { 'F', 0, 0, 0 } },                  // ARMENIAN CAPITAL LETTER FEH
        { 0x0559, { '<', 0, 0, 0 } },                  // ARMENIAN MODIFIER LETTER LEFT HALF RING
        { 0x0561, { 'a', 0, 0, 0 } },                  // ARMENIAN SMALL LETTER AYB
        { 0x0562, { 'b', 0, 0, 0 } },                  // ARMENIAN SMALL LETTER BEN
        { 0x0563, { 'g', 0, 0, 0 } },                  // ARMENIAN SMALL LETTER GIM
        { 0x0564, { 'd', 0, 0, 0 } },                  // ARMENIAN SMALL LETTER DA
        { 0x0565, { 'e', 0, 0, 0 } },                  // ARMENIAN SMALL LETTER ECH
        { 0x0566, { 'z', 0, 0, 0 } },                  // ARMENIAN SMALL LETTER ZA
        { 0x0567, { 'e', 0, 0, 0 } },                  // ARMENIAN SMALL LETTER EH
        { 0x0568, { 'e', 0, 0, 0 } },                  // ARMENIAN SMALL LETTER ET
        { 0x0569, { 't', '`', 0, 0 } },                // ARMENIAN SMALL LETTER TO
        { 0x056A, { 'z', 'h', 0, 0 } },                // ARMENIAN SMALL LETTER ZHE
        { 0x056B, { 'i', 0, 0, 0 } },                  // ARMENIAN SMALL LETTER INI
        { 0x056C, { 'l', 0, 0, 0 } },                  // ARMENIAN SMALL LETTER LIWN
        { 0x056D, { 'k', 'h', 0, 0 } },                // ARMENIAN SMALL LETTER XEH
        { 0x056E, { 't', 's', 0, 0 } },                // ARMENIAN SMALL LETTER CA
        { 0x056F, { 'k', 0, 0, 0 } },                  // ARMENIAN SMALL LETTER KEN
        { 0x0570, { 'h', 0, 0, 0 } },                  // ARMENIAN SMALL LETTER HO
        { 0x0571, { 'd', 'z', 0, 0 } },                // ARMENIAN SMALL LETTER JA
        { 0x0572, { 'g', 'h', 0, 0 } },                // ARMENIAN SMALL LETTER GHAD
        { 0x0573, { 'c', 'h', 0, 0 } },                // ARMENIAN SMALL LETTER CHEH
        { 0x0574, { 'm', 0, 0, 0 } },                  // ARMENIAN SMALL LETTER MEN
        { 0x0575, { 'y', 0, 0, 0 } },                  // ARMENIAN SMALL LETTER YI
        { 0x0576, { 'n', 0, 0, 0 } },                  // ARMENIAN SMALL LETTER NOW
        { 0x0577, { 's', 'h', 0, 0 } },                // ARMENIAN SMALL LETTER SHA
        { 0x0578, { 'o', 0, 0, 0 } },                  // ARMENIAN SMALL LETTER VO
        { 0x0579, { 'c', 'h', '`', 0 } },              // ARMENIAN SMALL LETTER CHA
        { 0x057A, { 'p', 0, 0, 0 } },                  // ARMENIAN SMALL LETTER PEH
        { 0x057B, { 'j', 0, 0, 0 } },                  // ARMENIAN SMALL LETTER JHEH
        { 0x057C, { 'r', 'h', 0, 0 } },                // ARMENIAN SMALL LETTER RA
        { 0x057D, { 's', 0, 0, 0 } },                  // ARMENIAN SMALL LETTER SEH
        { 0x057E, { 'v', 0, 0, 0 } },                  // ARMENIAN SMALL LETTER VEW
        { 0x057F, { 't', 0, 0, 0 } },                  // ARMENIAN SMALL LETTER TIWN
        { 0x0580, { 'r', 0, 0, 0 } },                  // ARMENIAN SMALL LETTER REH
        { 0x0581, { 't', 's', '`', 0 } },              // ARMENIAN SMALL LETTER CO
        { 0x0582, { 'w', 0, 0, 0 } },                  // ARMENIAN SMALL LETTER YIWN
        { 0x0583, { 'p', '`', 0, 0 } },                // ARMENIAN SMALL LETTER PIWR
        { 0x0584, { 'k', '`', 0, 0 } },                // ARMENIAN SMALL LETTER KEH
        { 0x0585, { 'o', 0, 0, 0 } },                  // ARMENIAN SMALL LETTER OH
        { 0x0586, { 'f', 0, 0, 0 } },                  // ARMENIAN SMALL LETTER FEH
        { 0x0587, { 'e', 'w', 0, 0 } },                // ARMENIAN SMALL LIGATURE ECH YIWN
        { 0x05D1, { 'b', 0, 0, 0 } },                  // HEBREW LETTER BET
        { 0x05D2, { 'g', 0, 0, 0 } },                  // HEBREW LETTER GIMEL
        { 0x05D3, { 'd', 0, 0, 0 } },                  // HEBREW LETTER DALET
        { 0x05D4, { 'h', 0, 0, 0 } },                  // HEBREW LETTER HE
        { 0x05D5, { 'v', 0, 0, 0 } },                  // HEBREW LETTER VAV
        { 0x05D6, { 'z', 0, 0, 0 } },                  // HEBREW LETTER ZAYIN
        { 0x05D7, { 'k', 'h', 0, 0 } },                // HEBREW LETTER HET
        { 0x05D8, { 't', 0, 0, 0 } },                  // HEBREW LETTER TET
        { 0x05D9, { 'y', 0, 0, 0 } },                  // HEBREW LETTER YOD
        { 0x05DA, { 'k', 0, 0, 0 } },                  // HEBREW LETTER FINAL KAF
        { 0x05DB, { 'k', 0, 0, 0 } },                  // HEBREW LETTER KAF
        { 0x05DC, { 'l', 0, 0, 0 } },                  // HEBREW LETTER LAMED
        { 0x05DD, { 'm', 0, 0, 0 } },                  // HEBREW LETTER FINAL MEM
        { 0x05DE, { 'm', 0, 0, 0 } },                  // HEBREW LETTER MEM
        { 0x05DF, { 'n', 0, 0, 0 } },                  // HEBREW LETTER FINAL NUN
        { 0x05E0, { 'n', 0, 0, 0 } },                  // HEBREW LETTER NUN
        { 0x05E1, { 's', 0, 0, 0 } },                  // HEBREW LETTER SAMEKH
        { 0x05E2, { '`', 0, 0, 0 } },                  // HEBREW LETTER AYIN
        { 0x05E3, { 'p', 0, 0, 0 } },                  // HEBREW LETTER FINAL PE
        { 0x05E4, { 'p', 0, 0, 0 } },                  // HEBREW LETTER PE
        { 0x05E5, { 't', 's', 0, 0 } },                // HEBREW LETTER FINAL TSADI
        { 0x05E6, { 't', 's', 0, 0 } },                // HEBREW LETTER TSADI
        { 0x05E7, { 'q', 0, 0, 0 } },                  // HEBREW LETTER QOF
        { 0x05E8, { 'r', 0, 0, 0 } },                  // HEBREW LETTER RESH
        { 0x05E9, { 's', 'h', 0, 0 } },                // HEBREW LETTER SHIN
        { 0x05EA, { 't', 0, 0, 0 } },                  // HEBREW LETTER TAV
        { 0x05F0, { 'V', 0, 0, 0 } },                  // HEBREW LIGATURE YIDDISH DOUBLE VAV
        { 0x05F1, { 'o', 'y', 0, 0 } },                // HEBREW LIGATURE YIDDISH VAV YOD
        { 0x05F2, { 'i', 0, 0, 0 } },                  // HEBREW LIGATURE YIDDISH DOUBLE YOD
        { 0x2044, { '/', 0, 0, 0 } },                  // FRACTION SLASH
        { 0x2070, { '0', 0, 0, 0 } },                  // SUPERSCRIPT ZERO
        { 0x2074, { '4', 0, 0, 0 } },                  // SUPERSCRIPT FOUR
        { 0x2075, { '5', 0, 0, 0 } },                  // SUPERSCRIPT FIVE
        { 0x2076, { '6', 0, 0, 0 } },                  // SUPERSCRIPT SIX
        { 0x2077, { '7', 0, 0, 0 } },                  // SUPERSCRIPT SEVEN
        { 0x2078, { '8', 0, 0, 0 } },                  // SUPERSCRIPT EIGHT
        { 0x2079, { '9', 0, 0, 0 } },                  // SUPERSCRIPT NINE
        { 0x207A, { '+', 0, 0, 0 } },                  // SUPERSCRIPT PLUS SIGN
        { 0x207B, { '-', 0, 0, 0 } },                  // SUPERSCRIPT MINUS
        { 0x207C, { '=', 0, 0, 0 } },                  // SUPERSCRIPT EQUALS SIGN
        { 0x207F, { 'n', 0, 0, 0 } },                  // SUPERSCRIPT LATIN SMALL LETTER N
        { 0x2080, { '0', 0, 0, 0 } },                  // SUBSCRIPT ZERO
        { 0x2081, { '1', 0, 0, 0 } },                  // SUBSCRIPT ONE
        { 0x2082, { '2', 0, 0, 0 } },                  // SUBSCRIPT TWO
        { 0x2083, { '3', 0, 0, 0 } },                  // SUBSCRIPT THREE
        { 0x2084, { '4', 0, 0, 0 } },                  // SUBSCRIPT FOUR
        { 0x2085, { '5', 0, 0, 0 } },                  // SUBSCRIPT FIVE
        { 0x2086, { '6', 0, 0, 0 } },                  // SUBSCRIPT SIX
        { 0x2087, { '7', 0, 0, 0 } },                  // SUBSCRIPT SEVEN
        { 0x2088, { '8', 0, 0, 0 } },                  // SUBSCRIPT EIGHT
        { 0x2089, { '9', 0, 0, 0 } },                  // SUBSCRIPT NINE
        { 0x208A, { '+', 0, 0, 0 } },                  // SUBSCRIPT PLUS SIGN
        { 0x208B, { '-', 0, 0, 0 } },                  // SUBSCRIPT MINUS
        { 0x208C, { '=', 0, 0, 0 } },                  // SUBSCRIPT EQUALS SIGN
        { 0x20A0, { 'E', 'C', 'U', 0 } },              // EURO-CURRENCY SIGN
        { 0x20A1, { 'C', 'L', 0, 0 } },                // COLON SIGN
        { 0x20A2, { 'C', 'r', 0, 0 } },                // CRUZEIRO SIGN
        { 0x20A3, { 'F', 'F', 0, 0 } },                // FRENCH FRANC SIGN
        { 0x20A4, { 'L', 0, 0, 0 } },                  // LIRA SIGN
        { 0x20A5, { 'm', 'i', 'l', 0 } },              // MILL SIGN
        { 0x20A6, { 'N', 0, 0, 0 } },                  // NAIRA SIGN
        { 0x20A7, { 'P', 't', 's', 0 } },              // PESETA SIGN
        { 0x20A8, { 'R', 's', 0, 0 } },                // RUPEE SIGN
        { 0x20A9, { 'W', 0, 0, 0 } },                  // WON SIGN
        { 0x20AA, { 'N', 'S', 0, 0 } },                // NEW SHEQEL SIGN
        { 0x20AB, { 'D', 0, 0, 0 } },                  // DONG SIGN
        { 0x20AC, { 'E', 'U', 0, 0 } },                // EURO SIGN
        { 0x20AD, { 'K', 0, 0, 0 } },                  // KIP SIGN
        { 0x20AE, { 'T', 0, 0, 0 } },                  // TUGRIK SIGN
        { 0x20AF, { 'D', 'r', 0, 0 } },                // DRACHMA SIGN
        { 0x212A, { 'K', 0, 0, 0 } },                  // KELVIN SIGN
        { 0x212B, { 'A', 0, 0, 0 } },                  // ANGSTROM SIGN
        { 0x2132, { 'F', 0, 0, 0 } },                  // TURNED CAPITAL F
        { 0x214E, { 'F', 0, 0, 0 } },                  // TURNED SMALL F
        { 0x2153, { '1', '/', '3', 0 } },              // VULGAR FRACTION ONE THIRD
        { 0x2154, { '2', '/', '3', 0 } },              // VULGAR FRACTION TWO THIRDS
        { 0x2155, { '1', '/', '5', 0 } },              // VULGAR FRACTION ONE FIFTH
        { 0x2156, { '2', '/', '5', 0 } },              // VULGAR FRACTION TWO FIFTHS
        { 0x2157, { '3', '/', '5', 0 } },              // VULGAR FRACTION THREE FIFTHS
        { 0x2158, { '4', '/', '5', 0 } },              // VULGAR FRACTION FOUR FIFTHS
        { 0x2159, { '1', '/', '6', 0 } },              // VULGAR FRACTION ONE SIXTH
        { 0x215A, { '5', '/', '6', 0 } },              // VULGAR FRACTION FIVE SIXTHS
        { 0x215B, { '1', '/', '8', 0 } },              // VULGAR FRACTION ONE EIGHTH
        { 0x215C, { '3', '/', '8', 0 } },              // VULGAR FRACTION THREE EIGHTHS
        { 0x215D, { '5', '/', '8', 0 } },              // VULGAR FRACTION FIVE EIGHTHS
        { 0x215E, { '7', '/', '8', 0 } },              // VULGAR FRACTION SEVEN EIGHTHS
        { 0x215F, { '1', '/', 0, 0 } },                // FRACTION NUMERATOR ONE
        { 0x2183, { ')', 0, 0, 0 } },                  // ROMAN NUMERAL REVERSED ONE HUNDRED
        { 0x2190, { '-', 0, 0, 0 } },                  // LEFTWARDS ARROW
        { 0x2191, { '|', 0, 0, 0 } },                  // UPWARDS ARROW
        { 0x2192, { '-', 0, 0, 0 } },                  // RIGHTWARDS ARROW
        { 0x2193, { '|', 0, 0, 0 } },                  // DOWNWARDS ARROW
        { 0x2194, { '-', 0, 0, 0 } },                  // LEFT RIGHT ARROW
        { 0x219A, { '-', 0, 0, 0 } },                  // LEFTWARDS ARROW WITH STROKE
        { 0x219B, { '-', 0, 0, 0 } },                  // RIGHTWARDS ARROW WITH STROKE
        { 0x21A0, { '-', 0, 0, 0 } },                  // RIGHTWARDS TWO HEADED ARROW
        { 0x21A3, { '-', 0, 0, 0 } },                  // RIGHTWARDS ARROW WITH TAIL
        { 0x21A6, { '-', 0, 0, 0 } },                  // RIGHTWARDS ARROW FROM BAR
        { 0x21AE, { '-', 0, 0, 0 } },                  // LEFT RIGHT ARROW WITH STROKE
        { 0x21CE, { '=', 0, 0, 0 } },                  // LEFT RIGHT DOUBLE ARROW WITH STROKE
        { 0x21CF, { '=', 0, 0, 0 } },                  // RIGHTWARDS DOUBLE ARROW WITH STROKE
        { 0x21D2, { '=', 0, 0, 0 } },                  // RIGHTWARDS DOUBLE ARROW
        { 0x21D4, { '=', 0, 0, 0 } },                  // LEFT RIGHT DOUBLE ARROW
        { 0x2260, { '!', '=', 0, 0 } },                // NOT EQUAL TO
        { 0x2264, { '<', '=', 0, 0 } },                // LESS-THAN OR EQUAL TO
        { 0x2265, { '>', '=', 0, 0 } },                // GREATER-THAN OR EQUAL TO
        { 0x24EA, { '0', 0, 0, 0 } },                  // CIRCLED DIGIT ZERO
        { 0x25B7, { '>', 0, 0, 0 } },                  // WHITE RIGHT-POINTING TRIANGLE
        { 0x25C1, { '<', 0, 0, 0 } },                  // WHITE LEFT-POINTING TRIANGLE
    };

    const Element* findTransliteration(afl::charset::Unichar_t ch,
                                       afl::base::Memory<const Element> data)
    {
        // Binary search
        while (data.size() > 3) {
            size_t mid = data.size() / 2;
            const Element* ele = data.at(mid);
            assert(ele != 0);
            if (ch < ele->code) {
                data.trim(mid);
            } else {
                data.split(mid);
            }
        }

        while (const Element* ele = data.eat()) {
            if (ele->code == ch) {
                return ele;
            }
        }
        return 0;
    }
}


afl::charset::AsciiTransliterator::~AsciiTransliterator()
{ }

afl::base::GrowableBytes_t
afl::charset::AsciiTransliterator::encode(afl::string::ConstStringMemory_t in)
{
    afl::base::GrowableBytes_t result;
    result.append(in.toBytes());
    return result;
}

String_t
afl::charset::AsciiTransliterator::decode(afl::base::ConstBytes_t in)
{
    String_t result;
    Utf8Reader reader(in, 0);
    while (reader.hasMore()) {
        Unichar_t ch = reader.eat();
        if (ch == 0 || isErrorCharacter(ch)) {
            // Ignore
        } else if (ch <= 0x7F) {
            // ASCII
            result += char(ch);
        } else if (const Element* ele = findTransliteration(ch, TRANSLITERATION)) {
            // Found
            for (size_t i = 0; i < MAX_CHARS && ele->transliteration[i] != '\0'; ++i) {
                result += ele->transliteration[i];
            }
        } else {
            // Not transliterable - ignore
        }
    }
    return result;
}

afl::charset::AsciiTransliterator*
afl::charset::AsciiTransliterator::clone() const
{
    return new AsciiTransliterator();
}
