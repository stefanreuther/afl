/**
  *  \file u/t_io_json_parser_testsuite.cpp
  *  \brief Test for afl::io::json::Parser - Testsuite
  *
  *  GENERATED FILE, do not edit. See scripts/generate_json_testsuite.pl.
  *
  *  Uses the test suite from https://github.com/nst/JSONTestSuite which comes with this license:
  *
  *  MIT License
  *
  *  Copyright (c) 2016 Nicolas Seriot
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

#include "afl/io/json/parser.hpp"

#include "u/t_io_json.hpp"
#include "afl/data/defaultvaluefactory.hpp"
#include "afl/data/value.hpp"
#include "afl/except/fileformatexception.hpp"
#include "afl/io/bufferedstream.hpp"
#include "afl/io/constmemorystream.hpp"

namespace {
    void parseString(afl::base::ConstBytes_t str)
    {
        afl::data::DefaultValueFactory factory;
        afl::io::ConstMemoryStream cms(str);
        afl::io::BufferedStream bs(cms);
        delete afl::io::json::Parser(bs, factory).parseComplete();
    }
}

/** Test "good" cases. These must all parse successfully. */
void
TestIoJsonParser::testTestsuiteGood()
{
    static const uint8_t y_array_arraysWithSpaces[] = {
        '[', '[', ']', ' ', ' ', ' ', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_array_arraysWithSpaces));

    static const uint8_t y_array_empty_string[] = {
        '[', '"', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_array_empty_string));

    static const uint8_t y_array_empty[] = {
        '[', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_array_empty));

    static const uint8_t y_array_ending_with_newline[] = {
        '[', '"', 'a', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_array_ending_with_newline));

    static const uint8_t y_array_false[] = {
        '[', 'f', 'a', 'l', 's', 'e', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_array_false));

    static const uint8_t y_array_heterogeneous[] = {
        '[', 'n', 'u', 'l', 'l', ',', ' ', '1', ',', ' ', '"', '1', '"', ',', ' ', '{', '}', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_array_heterogeneous));

    static const uint8_t y_array_null[] = {
        '[', 'n', 'u', 'l', 'l', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_array_null));

    static const uint8_t y_array_with_1_and_newline[] = {
        '[', '1',  10, ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_array_with_1_and_newline));

    static const uint8_t y_array_with_leading_space[] = {
        ' ', '[', '1', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_array_with_leading_space));

    static const uint8_t y_array_with_several_null[] = {
        '[', '1', ',', 'n', 'u', 'l', 'l', ',', 'n', 'u', 'l', 'l', ',', 'n', 'u', 'l', 'l', ',', '2', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_array_with_several_null));

    static const uint8_t y_array_with_trailing_space[] = {
        '[', '2', ']', ' ',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_array_with_trailing_space));

    static const uint8_t y_number[] = {
        '[', '1', '2', '3', 'e', '6', '5', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_number));

    static const uint8_t y_number_0e_1[] = {
        '[', '0', 'e', '+', '1', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_number_0e_1));

    static const uint8_t y_number_0e1[] = {
        '[', '0', 'e', '1', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_number_0e1));

    static const uint8_t y_number_after_space[] = {
        '[', ' ', '4', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_number_after_space));

    static const uint8_t y_number_double_close_to_zero[] = {
        '[', '-', '0', '.', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
        '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
        '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
        '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
        '0', '1', ']',  10,
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_number_double_close_to_zero));

    static const uint8_t y_number_int_with_exp[] = {
        '[', '2', '0', 'e', '1', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_number_int_with_exp));

    static const uint8_t y_number_minus_zero[] = {
        '[', '-', '0', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_number_minus_zero));

    static const uint8_t y_number_negative_int[] = {
        '[', '-', '1', '2', '3', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_number_negative_int));

    static const uint8_t y_number_negative_one[] = {
        '[', '-', '1', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_number_negative_one));

    static const uint8_t y_number_negative_zero[] = {
        '[', '-', '0', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_number_negative_zero));

    static const uint8_t y_number_real_capital_e[] = {
        '[', '1', 'E', '2', '2', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_number_real_capital_e));

    static const uint8_t y_number_real_capital_e_neg_exp[] = {
        '[', '1', 'E', '-', '2', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_number_real_capital_e_neg_exp));

    static const uint8_t y_number_real_capital_e_pos_exp[] = {
        '[', '1', 'E', '+', '2', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_number_real_capital_e_pos_exp));

    static const uint8_t y_number_real_exponent[] = {
        '[', '1', '2', '3', 'e', '4', '5', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_number_real_exponent));

    static const uint8_t y_number_real_fraction_exponent[] = {
        '[', '1', '2', '3', '.', '4', '5', '6', 'e', '7', '8', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_number_real_fraction_exponent));

    static const uint8_t y_number_real_neg_exp[] = {
        '[', '1', 'e', '-', '2', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_number_real_neg_exp));

    static const uint8_t y_number_real_pos_exponent[] = {
        '[', '1', 'e', '+', '2', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_number_real_pos_exponent));

    static const uint8_t y_number_simple_int[] = {
        '[', '1', '2', '3', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_number_simple_int));

    static const uint8_t y_number_simple_real[] = {
        '[', '1', '2', '3', '.', '4', '5', '6', '7', '8', '9', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_number_simple_real));

    static const uint8_t y_number_too_big_neg_int[] = {
        '[', '-', '1', '2', '3', '1', '2', '3', '1', '2', '3', '1', '2', '3', '1', '2', '3', '1', '2', '3',
        '1', '2', '3', '1', '2', '3', '1', '2', '3', '1', '2', '3', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_number_too_big_neg_int));

    static const uint8_t y_number_too_big_pos_int[] = {
        '[', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
        '0', '0', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_number_too_big_pos_int));

    static const uint8_t y_number_very_big_negative_int[] = {
        '[', '-', '2', '3', '7', '4', '6', '2', '3', '7', '4', '6', '7', '3', '2', '7', '6', '8', '9', '4',
        '2', '7', '9', '8', '3', '2', '7', '4', '9', '8', '3', '2', '4', '2', '3', '4', '7', '9', '8', '2',
        '3', '2', '4', '6', '3', '2', '7', '8', '4', '6', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_number_very_big_negative_int));

    static const uint8_t y_object[] = {
        '{', '"', 'a', 's', 'd', '"', ':', '"', 's', 'd', 'f', '"', ',', ' ', '"', 'd', 'f', 'g', '"', ':',
        '"', 'f', 'g', 'h', '"', '}',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_object));

    static const uint8_t y_object_basic[] = {
        '{', '"', 'a', 's', 'd', '"', ':', '"', 's', 'd', 'f', '"', '}',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_object_basic));

    static const uint8_t y_object_duplicated_key[] = {
        '{', '"', 'a', '"', ':', '"', 'b', '"', ',', '"', 'a', '"', ':', '"', 'c', '"', '}',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_object_duplicated_key));

    static const uint8_t y_object_duplicated_key_and_value[] = {
        '{', '"', 'a', '"', ':', '"', 'b', '"', ',', '"', 'a', '"', ':', '"', 'b', '"', '}',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_object_duplicated_key_and_value));

    static const uint8_t y_object_empty[] = {
        '{', '}',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_object_empty));

    static const uint8_t y_object_empty_key[] = {
        '{', '"', '"', ':', '0', '}',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_object_empty_key));

    static const uint8_t y_object_escaped_null_in_key[] = {
        '{', '"', 'f', 'o', 'o',  92, 'u', '0', '0', '0', '0', 'b', 'a', 'r', '"', ':', ' ', '4', '2', '}',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_object_escaped_null_in_key));

    static const uint8_t y_object_extreme_numbers[] = {
        '{', ' ', '"', 'm', 'i', 'n', '"', ':', ' ', '-', '1', '.', '0', 'e', '+', '2', '8', ',', ' ', '"',
        'm', 'a', 'x', '"', ':', ' ', '1', '.', '0', 'e', '+', '2', '8', ' ', '}',  10,
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_object_extreme_numbers));

    static const uint8_t y_object_long_strings[] = {
        '{', '"', 'x', '"', ':', '[', '{', '"', 'i', 'd', '"', ':', ' ', '"', 'x', 'x', 'x', 'x', 'x', 'x',
        'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
        'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', '"', '}', ']', ',', ' ', '"',
        'i', 'd', '"', ':', ' ', '"', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
        'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
        'x', 'x', 'x', 'x', 'x', 'x', '"', '}',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_object_long_strings));

    static const uint8_t y_object_simple[] = {
        '{', '"', 'a', '"', ':', '[', ']', '}',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_object_simple));

    static const uint8_t y_object_string_unicode[] = {
        '{', '"', 't', 'i', 't', 'l', 'e', '"', ':', '"',  92, 'u', '0', '4', '1', 'f',  92, 'u', '0', '4',
        '3', 'e',  92, 'u', '0', '4', '3', 'b',  92, 'u', '0', '4', '4', '2',  92, 'u', '0', '4', '3', 'e',
         92, 'u', '0', '4', '4', '0',  92, 'u', '0', '4', '3', '0', ' ',  92, 'u', '0', '4', '1', '7',  92,
        'u', '0', '4', '3', '5',  92, 'u', '0', '4', '3', 'c',  92, 'u', '0', '4', '3', 'b',  92, 'u', '0',
        '4', '3', '5',  92, 'u', '0', '4', '3', 'a',  92, 'u', '0', '4', '3', 'e',  92, 'u', '0', '4', '3',
        'f',  92, 'u', '0', '4', '3', '0', '"', ' ', '}',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_object_string_unicode));

    static const uint8_t y_object_with_newlines[] = {
        '{',  10, '"', 'a', '"', ':', ' ', '"', 'b', '"',  10, '}',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_object_with_newlines));

    static const uint8_t y_string_1_2_3_bytes_UTF_8_sequences[] = {
        '[', '"',  92, 'u', '0', '0', '6', '0',  92, 'u', '0', '1', '2', 'a',  92, 'u', '1', '2', 'A', 'B',
        '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_1_2_3_bytes_UTF_8_sequences));

    static const uint8_t y_string_UTF_16_Surrogates_U_1D11E_MUSICAL_SYMBOL_G_CLEF[] = {
        '[', '"',  92, 'u', 'D', '8', '3', '4',  92, 'u', 'D', 'd', '1', 'e', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_UTF_16_Surrogates_U_1D11E_MUSICAL_SYMBOL_G_CLEF));

    static const uint8_t y_string_accepted_surrogate_pair[] = {
        '[', '"',  92, 'u', 'D', '8', '0', '1',  92, 'u', 'd', 'c', '3', '7', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_accepted_surrogate_pair));

    static const uint8_t y_string_accepted_surrogate_pairs[] = {
        '[', '"',  92, 'u', 'd', '8', '3', 'd',  92, 'u', 'd', 'e', '3', '9',  92, 'u', 'd', '8', '3', 'd',
         92, 'u', 'd', 'c', '8', 'd', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_accepted_surrogate_pairs));

    static const uint8_t y_string_allowed_escapes[] = {
        '[', '"',  92, '"',  92,  92,  92, '/',  92, 'b',  92, 'f',  92, 'n',  92, 'r',  92, 't', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_allowed_escapes));

    static const uint8_t y_string_backslash_and_u_escaped_zero[] = {
        '[', '"',  92,  92, 'u', '0', '0', '0', '0', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_backslash_and_u_escaped_zero));

    static const uint8_t y_string_backslash_doublequotes[] = {
        '[', '"',  92, '"', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_backslash_doublequotes));

    static const uint8_t y_string_comments[] = {
        '[', '"', 'a', '/', '*', 'b', '*', '/', 'c', '/', '*', 'd', '/', '/', 'e', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_comments));

    static const uint8_t y_string_double_escape_a[] = {
        '[', '"',  92,  92, 'a', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_double_escape_a));

    static const uint8_t y_string_double_escape_n[] = {
        '[', '"',  92,  92, 'n', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_double_escape_n));

    static const uint8_t y_string_escaped_control_character[] = {
        '[', '"',  92, 'u', '0', '0', '1', '2', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_escaped_control_character));

    static const uint8_t y_string_escaped_noncharacter[] = {
        '[', '"',  92, 'u', 'F', 'F', 'F', 'F', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_escaped_noncharacter));

    static const uint8_t y_string_in_array[] = {
        '[', '"', 'a', 's', 'd', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_in_array));

    static const uint8_t y_string_in_array_with_leading_space[] = {
        '[', ' ', '"', 'a', 's', 'd', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_in_array_with_leading_space));

    static const uint8_t y_string_last_surrogates_1_and_2[] = {
        '[', '"',  92, 'u', 'D', 'B', 'F', 'F',  92, 'u', 'D', 'F', 'F', 'F', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_last_surrogates_1_and_2));

    static const uint8_t y_string_newline_uescaped[] = {
        '[', '"', 'n', 'e', 'w',  92, 'u', '0', '0', 'A', '0', 'l', 'i', 'n', 'e', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_newline_uescaped));

    static const uint8_t y_string_nonCharacterInUTF_8_U_10FFFF[] = {
        '[', '"', 244, 143, 191, 191, '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_nonCharacterInUTF_8_U_10FFFF));

    static const uint8_t y_string_nonCharacterInUTF_8_U_1FFFF[] = {
        '[', '"', 240, 155, 191, 191, '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_nonCharacterInUTF_8_U_1FFFF));

    static const uint8_t y_string_nonCharacterInUTF_8_U_FFFF[] = {
        '[', '"', 239, 191, 191, '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_nonCharacterInUTF_8_U_FFFF));

    static const uint8_t y_string_null_escape[] = {
        '[', '"',  92, 'u', '0', '0', '0', '0', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_null_escape));

    static const uint8_t y_string_one_byte_utf_8[] = {
        '[', '"',  92, 'u', '0', '0', '2', 'c', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_one_byte_utf_8));

    static const uint8_t y_string_pi[] = {
        '[', '"', 207, 128, '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_pi));

    static const uint8_t y_string_simple_ascii[] = {
        '[', '"', 'a', 's', 'd', ' ', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_simple_ascii));

    static const uint8_t y_string_space[] = {
        '"', ' ', '"',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_space));

    static const uint8_t y_string_three_byte_utf_8[] = {
        '[', '"',  92, 'u', '0', '8', '2', '1', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_three_byte_utf_8));

    static const uint8_t y_string_two_byte_utf_8[] = {
        '[', '"',  92, 'u', '0', '1', '2', '3', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_two_byte_utf_8));

    static const uint8_t y_string_u_2028_line_sep[] = {
        '[', '"', 226, 128, 168, '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_u_2028_line_sep));

    static const uint8_t y_string_u_2029_par_sep[] = {
        '[', '"', 226, 128, 169, '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_u_2029_par_sep));

    static const uint8_t y_string_uEscape[] = {
        '[', '"',  92, 'u', '0', '0', '6', '1',  92, 'u', '3', '0', 'a', 'f',  92, 'u', '3', '0', 'E', 'A',
         92, 'u', '3', '0', 'b', '9', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_uEscape));

    static const uint8_t y_string_unescaped_char_delete[] = {
        '[', '"', 127, '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_unescaped_char_delete));

    static const uint8_t y_string_unicode[] = {
        '[', '"',  92, 'u', 'A', '6', '6', 'D', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_unicode));

    static const uint8_t y_string_unicodeEscapedBackslash[] = {
        '[', '"',  92, 'u', '0', '0', '5', 'C', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_unicodeEscapedBackslash));

    static const uint8_t y_string_unicode_2[] = {
        '[', '"', 226, 141, 130, 227, 136, 180, 226, 141, 130, '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_unicode_2));

    static const uint8_t y_string_unicode_U_200B_ZERO_WIDTH_SPACE[] = {
        '[', '"',  92, 'u', '2', '0', '0', 'B', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_unicode_U_200B_ZERO_WIDTH_SPACE));

    static const uint8_t y_string_unicode_U_2064_invisible_plus[] = {
        '[', '"',  92, 'u', '2', '0', '6', '4', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_unicode_U_2064_invisible_plus));

    static const uint8_t y_string_unicode_escaped_double_quote[] = {
        '[', '"',  92, 'u', '0', '0', '2', '2', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_unicode_escaped_double_quote));

    static const uint8_t y_string_utf8[] = {
        '[', '"', 226, 130, 172, 240, 157, 132, 158, '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_utf8));

    static const uint8_t y_string_with_del_character[] = {
        '[', '"', 'a', 127, 'a', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_string_with_del_character));

    static const uint8_t y_structure_lonely_false[] = {
        'f', 'a', 'l', 's', 'e',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_structure_lonely_false));

    static const uint8_t y_structure_lonely_int[] = {
        '4', '2',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_structure_lonely_int));

    static const uint8_t y_structure_lonely_negative_real[] = {
        '-', '0', '.', '1',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_structure_lonely_negative_real));

    static const uint8_t y_structure_lonely_null[] = {
        'n', 'u', 'l', 'l',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_structure_lonely_null));

    static const uint8_t y_structure_lonely_string[] = {
        '"', 'a', 's', 'd', '"',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_structure_lonely_string));

    static const uint8_t y_structure_lonely_true[] = {
        't', 'r', 'u', 'e',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_structure_lonely_true));

    static const uint8_t y_structure_string_empty[] = {
        '"', '"',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_structure_string_empty));

    static const uint8_t y_structure_trailing_newline[] = {
        '[', '"', 'a', '"', ']',  10,
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_structure_trailing_newline));

    static const uint8_t y_structure_true_in_array[] = {
        '[', 't', 'r', 'u', 'e', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_structure_true_in_array));

    static const uint8_t y_structure_whitespace_array[] = {
        ' ', '[', ']', ' ',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(y_structure_whitespace_array));

}

/** Test "bad" cases. These must all fail. */
void
TestIoJsonParser::testTestsuiteBad()
{
    static const uint8_t n_array_1_true_without_comma[] = {
        '[', '1', ' ', 't', 'r', 'u', 'e', ']',
    };
    TS_ASSERT_THROWS(parseString(n_array_1_true_without_comma), afl::except::FileFormatException);

    static const uint8_t n_array_a_invalid_utf8[] = {
        '[', 'a', 229, ']',
    };
    TS_ASSERT_THROWS(parseString(n_array_a_invalid_utf8), afl::except::FileFormatException);

    static const uint8_t n_array_colon_instead_of_comma[] = {
        '[', '"', '"', ':', ' ', '1', ']',
    };
    TS_ASSERT_THROWS(parseString(n_array_colon_instead_of_comma), afl::except::FileFormatException);

    static const uint8_t n_array_comma_after_close[] = {
        '[', '"', '"', ']', ',',
    };
    TS_ASSERT_THROWS(parseString(n_array_comma_after_close), afl::except::FileFormatException);

    static const uint8_t n_array_comma_and_number[] = {
        '[', ',', '1', ']',
    };
    TS_ASSERT_THROWS(parseString(n_array_comma_and_number), afl::except::FileFormatException);

    static const uint8_t n_array_double_comma[] = {
        '[', '1', ',', ',', '2', ']',
    };
    TS_ASSERT_THROWS(parseString(n_array_double_comma), afl::except::FileFormatException);

    static const uint8_t n_array_double_extra_comma[] = {
        '[', '"', 'x', '"', ',', ',', ']',
    };
    TS_ASSERT_THROWS(parseString(n_array_double_extra_comma), afl::except::FileFormatException);

    static const uint8_t n_array_extra_close[] = {
        '[', '"', 'x', '"', ']', ']',
    };
    TS_ASSERT_THROWS(parseString(n_array_extra_close), afl::except::FileFormatException);

    static const uint8_t n_array_extra_comma[] = {
        '[', '"', '"', ',', ']',
    };
    TS_ASSERT_THROWS(parseString(n_array_extra_comma), afl::except::FileFormatException);

    static const uint8_t n_array_incomplete[] = {
        '[', '"', 'x', '"',
    };
    TS_ASSERT_THROWS(parseString(n_array_incomplete), afl::except::FileFormatException);

    static const uint8_t n_array_incomplete_invalid_value[] = {
        '[', 'x',
    };
    TS_ASSERT_THROWS(parseString(n_array_incomplete_invalid_value), afl::except::FileFormatException);

    static const uint8_t n_array_inner_array_no_comma[] = {
        '[', '3', '[', '4', ']', ']',
    };
    TS_ASSERT_THROWS(parseString(n_array_inner_array_no_comma), afl::except::FileFormatException);

    static const uint8_t n_array_invalid_utf8[] = {
        '[', 255, ']',
    };
    TS_ASSERT_THROWS(parseString(n_array_invalid_utf8), afl::except::FileFormatException);

    static const uint8_t n_array_items_separated_by_semicolon[] = {
        '[', '1', ':', '2', ']',
    };
    TS_ASSERT_THROWS(parseString(n_array_items_separated_by_semicolon), afl::except::FileFormatException);

    static const uint8_t n_array_just_comma[] = {
        '[', ',', ']',
    };
    TS_ASSERT_THROWS(parseString(n_array_just_comma), afl::except::FileFormatException);

    static const uint8_t n_array_just_minus[] = {
        '[', '-', ']',
    };
    TS_ASSERT_THROWS(parseString(n_array_just_minus), afl::except::FileFormatException);

    static const uint8_t n_array_missing_value[] = {
        '[', ' ', ' ', ' ', ',', ' ', '"', '"', ']',
    };
    TS_ASSERT_THROWS(parseString(n_array_missing_value), afl::except::FileFormatException);

    static const uint8_t n_array_newlines_unclosed[] = {
        '[', '"', 'a', '"', ',',  10, '4',  10, ',', '1', ',',
    };
    TS_ASSERT_THROWS(parseString(n_array_newlines_unclosed), afl::except::FileFormatException);

    static const uint8_t n_array_number_and_comma[] = {
        '[', '1', ',', ']',
    };
    TS_ASSERT_THROWS(parseString(n_array_number_and_comma), afl::except::FileFormatException);

    static const uint8_t n_array_number_and_several_commas[] = {
        '[', '1', ',', ',', ']',
    };
    TS_ASSERT_THROWS(parseString(n_array_number_and_several_commas), afl::except::FileFormatException);

    static const uint8_t n_array_spaces_vertical_tab_formfeed[] = {
        '[', '"',  11, 'a', '"',  92, 'f', ']',
    };
    TS_ASSERT_THROWS(parseString(n_array_spaces_vertical_tab_formfeed), afl::except::FileFormatException);

    static const uint8_t n_array_star_inside[] = {
        '[', '*', ']',
    };
    TS_ASSERT_THROWS(parseString(n_array_star_inside), afl::except::FileFormatException);

    static const uint8_t n_array_unclosed[] = {
        '[', '"', '"',
    };
    TS_ASSERT_THROWS(parseString(n_array_unclosed), afl::except::FileFormatException);

    static const uint8_t n_array_unclosed_trailing_comma[] = {
        '[', '1', ',',
    };
    TS_ASSERT_THROWS(parseString(n_array_unclosed_trailing_comma), afl::except::FileFormatException);

    static const uint8_t n_array_unclosed_with_new_lines[] = {
        '[', '1', ',',  10, '1',  10, ',', '1',
    };
    TS_ASSERT_THROWS(parseString(n_array_unclosed_with_new_lines), afl::except::FileFormatException);

    static const uint8_t n_array_unclosed_with_object_inside[] = {
        '[', '{', '}',
    };
    TS_ASSERT_THROWS(parseString(n_array_unclosed_with_object_inside), afl::except::FileFormatException);

    static const uint8_t n_incomplete_false[] = {
        '[', 'f', 'a', 'l', 's', ']',
    };
    TS_ASSERT_THROWS(parseString(n_incomplete_false), afl::except::FileFormatException);

    static const uint8_t n_incomplete_null[] = {
        '[', 'n', 'u', 'l', ']',
    };
    TS_ASSERT_THROWS(parseString(n_incomplete_null), afl::except::FileFormatException);

    static const uint8_t n_incomplete_true[] = {
        '[', 't', 'r', 'u', ']',
    };
    TS_ASSERT_THROWS(parseString(n_incomplete_true), afl::except::FileFormatException);

    static const uint8_t n_number__[] = {
        '[', '+', '+', '1', '2', '3', '4', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number__), afl::except::FileFormatException);

    static const uint8_t n_number__1[] = {
        '[', '+', '1', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number__1), afl::except::FileFormatException);

    static const uint8_t n_number__Inf[] = {
        '[', '+', 'I', 'n', 'f', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number__Inf), afl::except::FileFormatException);

    static const uint8_t n_number__1_0_[] = {
        '[', '-', '1', '.', '0', '.', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number__1_0_), afl::except::FileFormatException);

    static const uint8_t n_number__NaN[] = {
        '[', '-', 'N', 'a', 'N', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number__NaN), afl::except::FileFormatException);

    static const uint8_t n_number__1_1[] = {
        '[', '.', '-', '1', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number__1_1), afl::except::FileFormatException);

    static const uint8_t n_number__2e_3[] = {
        '[', '.', '2', 'e', '-', '3', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number__2e_3), afl::except::FileFormatException);

    static const uint8_t n_number_0_1_2[] = {
        '[', '0', '.', '1', '.', '2', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_0_1_2), afl::except::FileFormatException);

    static const uint8_t n_number_0_3e_[] = {
        '[', '0', '.', '3', 'e', '+', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_0_3e_), afl::except::FileFormatException);

    static const uint8_t n_number_0_3e[] = {
        '[', '0', '.', '3', 'e', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_0_3e), afl::except::FileFormatException);

    static const uint8_t n_number_0_capital_E_[] = {
        '[', '0', 'E', '+', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_0_capital_E_), afl::except::FileFormatException);

    static const uint8_t n_number_0_capital_E[] = {
        '[', '0', 'E', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_0_capital_E), afl::except::FileFormatException);

    static const uint8_t n_number_0e_[] = {
        '[', '0', 'e', '+', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_0e_), afl::except::FileFormatException);

    static const uint8_t n_number_0e[] = {
        '[', '0', 'e', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_0e), afl::except::FileFormatException);

    static const uint8_t n_number_1_0e_[] = {
        '[', '1', '.', '0', 'e', '+', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_1_0e_), afl::except::FileFormatException);

    static const uint8_t n_number_1_0e__1[] = {
        '[', '1', '.', '0', 'e', '-', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_1_0e__1), afl::except::FileFormatException);

    static const uint8_t n_number_1_0e[] = {
        '[', '1', '.', '0', 'e', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_1_0e), afl::except::FileFormatException);

    static const uint8_t n_number_1_000[] = {
        '[', '1', ' ', '0', '0', '0', '.', '0', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_1_000), afl::except::FileFormatException);

    static const uint8_t n_number_1eE2[] = {
        '[', '1', 'e', 'E', '2', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_1eE2), afl::except::FileFormatException);

    static const uint8_t n_number_9_e_[] = {
        '[', '9', '.', 'e', '+', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_9_e_), afl::except::FileFormatException);

    static const uint8_t n_number_Inf[] = {
        '[', 'I', 'n', 'f', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_Inf), afl::except::FileFormatException);

    static const uint8_t n_number_NaN[] = {
        '[', 'N', 'a', 'N', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_NaN), afl::except::FileFormatException);

    static const uint8_t n_number_U_FF11_fullwidth_digit_one[] = {
        '[', 239, 188, 145, ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_U_FF11_fullwidth_digit_one), afl::except::FileFormatException);

    static const uint8_t n_number_expression[] = {
        '[', '1', '+', '2', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_expression), afl::except::FileFormatException);

    static const uint8_t n_number_hex_1_digit[] = {
        '[', '0', 'x', '1', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_hex_1_digit), afl::except::FileFormatException);

    static const uint8_t n_number_hex_2_digits[] = {
        '[', '0', 'x', '4', '2', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_hex_2_digits), afl::except::FileFormatException);

    static const uint8_t n_number_infinity[] = {
        '[', 'I', 'n', 'f', 'i', 'n', 'i', 't', 'y', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_infinity), afl::except::FileFormatException);

    static const uint8_t n_number_invalid_[] = {
        '[', '0', 'e', '+', '-', '1', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_invalid_), afl::except::FileFormatException);

    static const uint8_t n_number_invalid_negative_real[] = {
        '[', '-', '1', '2', '3', '.', '1', '2', '3', 'f', 'o', 'o', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_invalid_negative_real), afl::except::FileFormatException);

    static const uint8_t n_number_invalid_utf_8_in_bigger_int[] = {
        '[', '1', '2', '3', 229, ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_invalid_utf_8_in_bigger_int), afl::except::FileFormatException);

    static const uint8_t n_number_invalid_utf_8_in_exponent[] = {
        '[', '1', 'e', '1', 229, ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_invalid_utf_8_in_exponent), afl::except::FileFormatException);

    static const uint8_t n_number_invalid_utf_8_in_int[] = {
        '[', '0', 229, ']',  10,
    };
    TS_ASSERT_THROWS(parseString(n_number_invalid_utf_8_in_int), afl::except::FileFormatException);

    static const uint8_t n_number_minus_infinity[] = {
        '[', '-', 'I', 'n', 'f', 'i', 'n', 'i', 't', 'y', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_minus_infinity), afl::except::FileFormatException);

    static const uint8_t n_number_minus_sign_with_trailing_garbage[] = {
        '[', '-', 'f', 'o', 'o', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_minus_sign_with_trailing_garbage), afl::except::FileFormatException);

    static const uint8_t n_number_minus_space_1[] = {
        '[', '-', ' ', '1', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_minus_space_1), afl::except::FileFormatException);

    static const uint8_t n_number_neg_with_garbage_at_end[] = {
        '[', '-', '1', 'x', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_neg_with_garbage_at_end), afl::except::FileFormatException);

    static const uint8_t n_number_real_garbage_after_e[] = {
        '[', '1', 'e', 'a', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_real_garbage_after_e), afl::except::FileFormatException);

    static const uint8_t n_number_real_with_invalid_utf8_after_e[] = {
        '[', '1', 'e', 229, ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_real_with_invalid_utf8_after_e), afl::except::FileFormatException);

    static const uint8_t n_number_starting_with_dot[] = {
        '[', '.', '1', '2', '3', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_starting_with_dot), afl::except::FileFormatException);

    static const uint8_t n_number_then_00[] = {
        '1',   0,
    };
    TS_ASSERT_THROWS(parseString(n_number_then_00), afl::except::FileFormatException);

    static const uint8_t n_number_with_alpha[] = {
        '[', '1', '.', '2', 'a', '-', '3', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_with_alpha), afl::except::FileFormatException);

    static const uint8_t n_number_with_alpha_char[] = {
        '[', '1', '.', '8', '0', '1', '1', '6', '7', '0', '0', '3', '3', '3', '7', '6', '5', '1', '4', 'H',
        '-', '3', '0', '8', ']',
    };
    TS_ASSERT_THROWS(parseString(n_number_with_alpha_char), afl::except::FileFormatException);

    static const uint8_t n_object_bad_value[] = {
        '[', '"', 'x', '"', ',', ' ', 't', 'r', 'u', 't', 'h', ']',
    };
    TS_ASSERT_THROWS(parseString(n_object_bad_value), afl::except::FileFormatException);

    static const uint8_t n_object_bracket_key[] = {
        '{', '[', ':', ' ', '"', 'x', '"', '}',  10,
    };
    TS_ASSERT_THROWS(parseString(n_object_bracket_key), afl::except::FileFormatException);

    static const uint8_t n_object_comma_instead_of_colon[] = {
        '{', '"', 'x', '"', ',', ' ', 'n', 'u', 'l', 'l', '}',
    };
    TS_ASSERT_THROWS(parseString(n_object_comma_instead_of_colon), afl::except::FileFormatException);

    static const uint8_t n_object_double_colon[] = {
        '{', '"', 'x', '"', ':', ':', '"', 'b', '"', '}',
    };
    TS_ASSERT_THROWS(parseString(n_object_double_colon), afl::except::FileFormatException);

    static const uint8_t n_object_emoji[] = {
        '{', 240, 159, 135, 168, 240, 159, 135, 173, '}',
    };
    TS_ASSERT_THROWS(parseString(n_object_emoji), afl::except::FileFormatException);

    static const uint8_t n_object_garbage_at_end[] = {
        '{', '"', 'a', '"', ':', '"', 'a', '"', ' ', '1', '2', '3', '}',
    };
    TS_ASSERT_THROWS(parseString(n_object_garbage_at_end), afl::except::FileFormatException);

    static const uint8_t n_object_key_with_single_quotes[] = {
        '{', 'k', 'e', 'y', ':', ' ',  39, 'v', 'a', 'l', 'u', 'e',  39, '}',
    };
    TS_ASSERT_THROWS(parseString(n_object_key_with_single_quotes), afl::except::FileFormatException);

    static const uint8_t n_object_missing_colon[] = {
        '{', '"', 'a', '"', ' ', 'b', '}',
    };
    TS_ASSERT_THROWS(parseString(n_object_missing_colon), afl::except::FileFormatException);

    static const uint8_t n_object_missing_key[] = {
        '{', ':', '"', 'b', '"', '}',
    };
    TS_ASSERT_THROWS(parseString(n_object_missing_key), afl::except::FileFormatException);

    static const uint8_t n_object_missing_semicolon[] = {
        '{', '"', 'a', '"', ' ', '"', 'b', '"', '}',
    };
    TS_ASSERT_THROWS(parseString(n_object_missing_semicolon), afl::except::FileFormatException);

    static const uint8_t n_object_missing_value[] = {
        '{', '"', 'a', '"', ':',
    };
    TS_ASSERT_THROWS(parseString(n_object_missing_value), afl::except::FileFormatException);

    static const uint8_t n_object_no_colon[] = {
        '{', '"', 'a', '"',
    };
    TS_ASSERT_THROWS(parseString(n_object_no_colon), afl::except::FileFormatException);

    static const uint8_t n_object_non_string_key[] = {
        '{', '1', ':', '1', '}',
    };
    TS_ASSERT_THROWS(parseString(n_object_non_string_key), afl::except::FileFormatException);

    static const uint8_t n_object_non_string_key_but_huge_number_instead[] = {
        '{', '9', '9', '9', '9', 'E', '9', '9', '9', '9', ':', '1', '}',
    };
    TS_ASSERT_THROWS(parseString(n_object_non_string_key_but_huge_number_instead), afl::except::FileFormatException);

    static const uint8_t n_object_pi_in_key_and_trailing_comma[] = {
        '{', '"', 185, '"', ':', '"', '0', '"', ',', '}',
    };
    TS_ASSERT_THROWS(parseString(n_object_pi_in_key_and_trailing_comma), afl::except::FileFormatException);

    static const uint8_t n_object_repeated_null_null[] = {
        '{', 'n', 'u', 'l', 'l', ':', 'n', 'u', 'l', 'l', ',', 'n', 'u', 'l', 'l', ':', 'n', 'u', 'l', 'l',
        '}',
    };
    TS_ASSERT_THROWS(parseString(n_object_repeated_null_null), afl::except::FileFormatException);

    static const uint8_t n_object_several_trailing_commas[] = {
        '{', '"', 'i', 'd', '"', ':', '0', ',', ',', ',', ',', ',', '}',
    };
    TS_ASSERT_THROWS(parseString(n_object_several_trailing_commas), afl::except::FileFormatException);

    static const uint8_t n_object_single_quote[] = {
        '{',  39, 'a',  39, ':', '0', '}',
    };
    TS_ASSERT_THROWS(parseString(n_object_single_quote), afl::except::FileFormatException);

    static const uint8_t n_object_trailing_comma[] = {
        '{', '"', 'i', 'd', '"', ':', '0', ',', '}',
    };
    TS_ASSERT_THROWS(parseString(n_object_trailing_comma), afl::except::FileFormatException);

    static const uint8_t n_object_trailing_comment[] = {
        '{', '"', 'a', '"', ':', '"', 'b', '"', '}', '/', '*', '*', '/',
    };
    TS_ASSERT_THROWS(parseString(n_object_trailing_comment), afl::except::FileFormatException);

    static const uint8_t n_object_trailing_comment_open[] = {
        '{', '"', 'a', '"', ':', '"', 'b', '"', '}', '/', '*', '*', '/', '/',
    };
    TS_ASSERT_THROWS(parseString(n_object_trailing_comment_open), afl::except::FileFormatException);

    static const uint8_t n_object_trailing_comment_slash_open[] = {
        '{', '"', 'a', '"', ':', '"', 'b', '"', '}', '/', '/',
    };
    TS_ASSERT_THROWS(parseString(n_object_trailing_comment_slash_open), afl::except::FileFormatException);

    static const uint8_t n_object_trailing_comment_slash_open_incomplete[] = {
        '{', '"', 'a', '"', ':', '"', 'b', '"', '}', '/',
    };
    TS_ASSERT_THROWS(parseString(n_object_trailing_comment_slash_open_incomplete), afl::except::FileFormatException);

    static const uint8_t n_object_two_commas_in_a_row[] = {
        '{', '"', 'a', '"', ':', '"', 'b', '"', ',', ',', '"', 'c', '"', ':', '"', 'd', '"', '}',
    };
    TS_ASSERT_THROWS(parseString(n_object_two_commas_in_a_row), afl::except::FileFormatException);

    static const uint8_t n_object_unquoted_key[] = {
        '{', 'a', ':', ' ', '"', 'b', '"', '}',
    };
    TS_ASSERT_THROWS(parseString(n_object_unquoted_key), afl::except::FileFormatException);

    static const uint8_t n_object_unterminated_value[] = {
        '{', '"', 'a', '"', ':', '"', 'a',
    };
    TS_ASSERT_THROWS(parseString(n_object_unterminated_value), afl::except::FileFormatException);

    static const uint8_t n_object_with_single_string[] = {
        '{', ' ', '"', 'f', 'o', 'o', '"', ' ', ':', ' ', '"', 'b', 'a', 'r', '"', ',', ' ', '"', 'a', '"',
        ' ', '}',
    };
    TS_ASSERT_THROWS(parseString(n_object_with_single_string), afl::except::FileFormatException);

    static const uint8_t n_object_with_trailing_garbage[] = {
        '{', '"', 'a', '"', ':', '"', 'b', '"', '}', '#',
    };
    TS_ASSERT_THROWS(parseString(n_object_with_trailing_garbage), afl::except::FileFormatException);

    static const uint8_t n_single_space[] = {
        ' ',
    };
    TS_ASSERT_THROWS(parseString(n_single_space), afl::except::FileFormatException);

    static const uint8_t n_string_1_surrogate_then_escape_u[] = {
        '[', '"',  92, 'u', 'D', '8', '0', '0',  92, 'u', '"', ']',
    };
    TS_ASSERT_THROWS(parseString(n_string_1_surrogate_then_escape_u), afl::except::FileFormatException);

    static const uint8_t n_string_1_surrogate_then_escape_u1[] = {
        '[', '"',  92, 'u', 'D', '8', '0', '0',  92, 'u', '1', '"', ']',
    };
    TS_ASSERT_THROWS(parseString(n_string_1_surrogate_then_escape_u1), afl::except::FileFormatException);

    static const uint8_t n_string_1_surrogate_then_escape_u1x[] = {
        '[', '"',  92, 'u', 'D', '8', '0', '0',  92, 'u', '1', 'x', '"', ']',
    };
    TS_ASSERT_THROWS(parseString(n_string_1_surrogate_then_escape_u1x), afl::except::FileFormatException);

    static const uint8_t n_string_1_surrogate_then_escape[] = {
        '[', '"',  92, 'u', 'D', '8', '0', '0',  92, '"', ']',
    };
    TS_ASSERT_THROWS(parseString(n_string_1_surrogate_then_escape), afl::except::FileFormatException);

    static const uint8_t n_string_UTF_16_incomplete_surrogate[] = {
        '[', '"',  92, 'u', 'D', '8', '3', '4',  92, 'u', 'D', 'd', '"', ']',
    };
    TS_ASSERT_THROWS(parseString(n_string_UTF_16_incomplete_surrogate), afl::except::FileFormatException);

    static const uint8_t n_string_accentuated_char_no_quotes[] = {
        '[', 195, 169, ']',
    };
    TS_ASSERT_THROWS(parseString(n_string_accentuated_char_no_quotes), afl::except::FileFormatException);

    static const uint8_t n_string_backslash_00[] = {
        '[', '"',  92,   0, '"', ']',
    };
    TS_ASSERT_THROWS(parseString(n_string_backslash_00), afl::except::FileFormatException);

    static const uint8_t n_string_escape_x[] = {
        '[', '"',  92, 'x', '0', '0', '"', ']',
    };
    TS_ASSERT_THROWS(parseString(n_string_escape_x), afl::except::FileFormatException);

    static const uint8_t n_string_escaped_backslash_bad[] = {
        '[', '"',  92,  92,  92, '"', ']',
    };
    TS_ASSERT_THROWS(parseString(n_string_escaped_backslash_bad), afl::except::FileFormatException);

    static const uint8_t n_string_escaped_ctrl_char_tab[] = {
        '[', '"',  92,   9, '"', ']',
    };
    TS_ASSERT_THROWS(parseString(n_string_escaped_ctrl_char_tab), afl::except::FileFormatException);

    static const uint8_t n_string_escaped_emoji[] = {
        '[', '"',  92, 240, 159, 140, 128, '"', ']',
    };
    TS_ASSERT_THROWS(parseString(n_string_escaped_emoji), afl::except::FileFormatException);

    static const uint8_t n_string_incomplete_escape[] = {
        '[', '"',  92, '"', ']',
    };
    TS_ASSERT_THROWS(parseString(n_string_incomplete_escape), afl::except::FileFormatException);

    static const uint8_t n_string_incomplete_escaped_character[] = {
        '[', '"',  92, 'u', '0', '0', 'A', '"', ']',
    };
    TS_ASSERT_THROWS(parseString(n_string_incomplete_escaped_character), afl::except::FileFormatException);

    static const uint8_t n_string_incomplete_surrogate_escape_invalid[] = {
        '[', '"',  92, 'u', 'D', '8', '0', '0',  92, 'u', 'D', '8', '0', '0',  92, 'x', '"', ']',
    };
    TS_ASSERT_THROWS(parseString(n_string_incomplete_surrogate_escape_invalid), afl::except::FileFormatException);

    static const uint8_t n_string_invalid_utf_8_in_escape[] = {
        '[', '"',  92, 'u', 229, '"', ']',
    };
    TS_ASSERT_THROWS(parseString(n_string_invalid_utf_8_in_escape), afl::except::FileFormatException);

    static const uint8_t n_string_invalid_backslash_esc[] = {
        '[', '"',  92, 'a', '"', ']',
    };
    TS_ASSERT_THROWS(parseString(n_string_invalid_backslash_esc), afl::except::FileFormatException);

    static const uint8_t n_string_invalid_unicode_escape[] = {
        '[', '"',  92, 'u', 'q', 'q', 'q', 'q', '"', ']',
    };
    TS_ASSERT_THROWS(parseString(n_string_invalid_unicode_escape), afl::except::FileFormatException);

    static const uint8_t n_string_invalid_utf8_after_escape[] = {
        '[', '"',  92, 229, '"', ']',
    };
    TS_ASSERT_THROWS(parseString(n_string_invalid_utf8_after_escape), afl::except::FileFormatException);

    static const uint8_t n_string_leading_uescaped_thinspace[] = {
        '[',  92, 'u', '0', '0', '2', '0', '"', 'a', 's', 'd', '"', ']',
    };
    TS_ASSERT_THROWS(parseString(n_string_leading_uescaped_thinspace), afl::except::FileFormatException);

    static const uint8_t n_string_no_quotes_with_bad_escape[] = {
        '[',  92, 'n', ']',
    };
    TS_ASSERT_THROWS(parseString(n_string_no_quotes_with_bad_escape), afl::except::FileFormatException);

    static const uint8_t n_string_single_doublequote[] = {
        '"',
    };
    TS_ASSERT_THROWS(parseString(n_string_single_doublequote), afl::except::FileFormatException);

    static const uint8_t n_string_single_quote[] = {
        '[',  39, 's', 'i', 'n', 'g', 'l', 'e', ' ', 'q', 'u', 'o', 't', 'e',  39, ']',
    };
    TS_ASSERT_THROWS(parseString(n_string_single_quote), afl::except::FileFormatException);

    static const uint8_t n_string_single_string_no_double_quotes[] = {
        'a', 'b', 'c',
    };
    TS_ASSERT_THROWS(parseString(n_string_single_string_no_double_quotes), afl::except::FileFormatException);

    static const uint8_t n_string_start_escape_unclosed[] = {
        '[', '"',  92,
    };
    TS_ASSERT_THROWS(parseString(n_string_start_escape_unclosed), afl::except::FileFormatException);

    static const uint8_t n_string_unescaped_crtl_char[] = {
        '[', '"', 'a',   0, 'a', '"', ']',
    };
    TS_ASSERT_THROWS(parseString(n_string_unescaped_crtl_char), afl::except::FileFormatException);

    static const uint8_t n_string_unescaped_newline[] = {
        '[', '"', 'n', 'e', 'w',  10, 'l', 'i', 'n', 'e', '"', ']',
    };
    TS_ASSERT_THROWS(parseString(n_string_unescaped_newline), afl::except::FileFormatException);

    static const uint8_t n_string_unescaped_tab[] = {
        '[', '"',   9, '"', ']',
    };
    TS_ASSERT_THROWS(parseString(n_string_unescaped_tab), afl::except::FileFormatException);

    static const uint8_t n_string_unicode_CapitalU[] = {
        '"',  92, 'U', 'A', '6', '6', 'D', '"',
    };
    TS_ASSERT_THROWS(parseString(n_string_unicode_CapitalU), afl::except::FileFormatException);

    static const uint8_t n_string_with_trailing_garbage[] = {
        '"', '"', 'x',
    };
    TS_ASSERT_THROWS(parseString(n_string_with_trailing_garbage), afl::except::FileFormatException);

    static const uint8_t n_structure__[] = {
        '<', '.', '>',
    };
    TS_ASSERT_THROWS(parseString(n_structure__), afl::except::FileFormatException);

    static const uint8_t n_structure__null_[] = {
        '[', '<', 'n', 'u', 'l', 'l', '>', ']',
    };
    TS_ASSERT_THROWS(parseString(n_structure__null_), afl::except::FileFormatException);

    static const uint8_t n_structure_U_2060_word_joined[] = {
        '[', 226, 129, 160, ']',
    };
    TS_ASSERT_THROWS(parseString(n_structure_U_2060_word_joined), afl::except::FileFormatException);

    static const uint8_t n_structure_UTF8_BOM_no_data[] = {
        239, 187, 191,
    };
    TS_ASSERT_THROWS(parseString(n_structure_UTF8_BOM_no_data), afl::except::FileFormatException);

    static const uint8_t n_structure_array_trailing_garbage[] = {
        '[', '1', ']', 'x',
    };
    TS_ASSERT_THROWS(parseString(n_structure_array_trailing_garbage), afl::except::FileFormatException);

    static const uint8_t n_structure_array_with_extra_array_close[] = {
        '[', '1', ']', ']',
    };
    TS_ASSERT_THROWS(parseString(n_structure_array_with_extra_array_close), afl::except::FileFormatException);

    static const uint8_t n_structure_array_with_unclosed_string[] = {
        '[', '"', 'a', 's', 'd', ']',
    };
    TS_ASSERT_THROWS(parseString(n_structure_array_with_unclosed_string), afl::except::FileFormatException);

    static const uint8_t n_structure_ascii_unicode_identifier[] = {
        'a', 195, 165,
    };
    TS_ASSERT_THROWS(parseString(n_structure_ascii_unicode_identifier), afl::except::FileFormatException);

    static const uint8_t n_structure_capitalized_True[] = {
        '[', 'T', 'r', 'u', 'e', ']',
    };
    TS_ASSERT_THROWS(parseString(n_structure_capitalized_True), afl::except::FileFormatException);

    static const uint8_t n_structure_close_unopened_array[] = {
        '1', ']',
    };
    TS_ASSERT_THROWS(parseString(n_structure_close_unopened_array), afl::except::FileFormatException);

    static const uint8_t n_structure_comma_instead_of_closing_brace[] = {
        '{', '"', 'x', '"', ':', ' ', 't', 'r', 'u', 'e', ',',
    };
    TS_ASSERT_THROWS(parseString(n_structure_comma_instead_of_closing_brace), afl::except::FileFormatException);

    static const uint8_t n_structure_double_array[] = {
        '[', ']', '[', ']',
    };
    TS_ASSERT_THROWS(parseString(n_structure_double_array), afl::except::FileFormatException);

    static const uint8_t n_structure_end_array[] = {
        ']',
    };
    TS_ASSERT_THROWS(parseString(n_structure_end_array), afl::except::FileFormatException);

    static const uint8_t n_structure_incomplete_UTF8_BOM[] = {
        239, 187, '{', '}',
    };
    TS_ASSERT_THROWS(parseString(n_structure_incomplete_UTF8_BOM), afl::except::FileFormatException);

    static const uint8_t n_structure_lone_invalid_utf_8[] = {
        229,
    };
    TS_ASSERT_THROWS(parseString(n_structure_lone_invalid_utf_8), afl::except::FileFormatException);

    static const uint8_t n_structure_lone_open_bracket[] = {
        '[',
    };
    TS_ASSERT_THROWS(parseString(n_structure_lone_open_bracket), afl::except::FileFormatException);

    static const uint8_t n_structure_null_byte_outside_string[] = {
        '[',   0, ']',
    };
    TS_ASSERT_THROWS(parseString(n_structure_null_byte_outside_string), afl::except::FileFormatException);

    static const uint8_t n_structure_number_with_trailing_garbage[] = {
        '2', '@',
    };
    TS_ASSERT_THROWS(parseString(n_structure_number_with_trailing_garbage), afl::except::FileFormatException);

    static const uint8_t n_structure_object_followed_by_closing_object[] = {
        '{', '}', '}',
    };
    TS_ASSERT_THROWS(parseString(n_structure_object_followed_by_closing_object), afl::except::FileFormatException);

    static const uint8_t n_structure_object_unclosed_no_value[] = {
        '{', '"', '"', ':',
    };
    TS_ASSERT_THROWS(parseString(n_structure_object_unclosed_no_value), afl::except::FileFormatException);

    static const uint8_t n_structure_object_with_comment[] = {
        '{', '"', 'a', '"', ':', '/', '*', 'c', 'o', 'm', 'm', 'e', 'n', 't', '*', '/', '"', 'b', '"', '}',
    };
    TS_ASSERT_THROWS(parseString(n_structure_object_with_comment), afl::except::FileFormatException);

    static const uint8_t n_structure_object_with_trailing_garbage[] = {
        '{', '"', 'a', '"', ':', ' ', 't', 'r', 'u', 'e', '}', ' ', '"', 'x', '"',
    };
    TS_ASSERT_THROWS(parseString(n_structure_object_with_trailing_garbage), afl::except::FileFormatException);

    static const uint8_t n_structure_open_array_apostrophe[] = {
        '[',  39,
    };
    TS_ASSERT_THROWS(parseString(n_structure_open_array_apostrophe), afl::except::FileFormatException);

    static const uint8_t n_structure_open_array_comma[] = {
        '[', ',',
    };
    TS_ASSERT_THROWS(parseString(n_structure_open_array_comma), afl::except::FileFormatException);

    static const uint8_t n_structure_open_array_open_object[] = {
        '[', '{',
    };
    TS_ASSERT_THROWS(parseString(n_structure_open_array_open_object), afl::except::FileFormatException);

    static const uint8_t n_structure_open_array_open_string[] = {
        '[', '"', 'a',
    };
    TS_ASSERT_THROWS(parseString(n_structure_open_array_open_string), afl::except::FileFormatException);

    static const uint8_t n_structure_open_array_string[] = {
        '[', '"', 'a', '"',
    };
    TS_ASSERT_THROWS(parseString(n_structure_open_array_string), afl::except::FileFormatException);

    static const uint8_t n_structure_open_object[] = {
        '{',
    };
    TS_ASSERT_THROWS(parseString(n_structure_open_object), afl::except::FileFormatException);

    static const uint8_t n_structure_open_object_close_array[] = {
        '{', ']',
    };
    TS_ASSERT_THROWS(parseString(n_structure_open_object_close_array), afl::except::FileFormatException);

    static const uint8_t n_structure_open_object_comma[] = {
        '{', ',',
    };
    TS_ASSERT_THROWS(parseString(n_structure_open_object_comma), afl::except::FileFormatException);

    static const uint8_t n_structure_open_object_open_array[] = {
        '{', '[',
    };
    TS_ASSERT_THROWS(parseString(n_structure_open_object_open_array), afl::except::FileFormatException);

    static const uint8_t n_structure_open_object_open_string[] = {
        '{', '"', 'a',
    };
    TS_ASSERT_THROWS(parseString(n_structure_open_object_open_string), afl::except::FileFormatException);

    static const uint8_t n_structure_open_object_string_with_apostrophes[] = {
        '{',  39, 'a',  39,
    };
    TS_ASSERT_THROWS(parseString(n_structure_open_object_string_with_apostrophes), afl::except::FileFormatException);

    static const uint8_t n_structure_open_open[] = {
        '[', '"',  92, '{', '[', '"',  92, '{', '[', '"',  92, '{', '[', '"',  92, '{',
    };
    TS_ASSERT_THROWS(parseString(n_structure_open_open), afl::except::FileFormatException);

    static const uint8_t n_structure_single_point[] = {
        233,
    };
    TS_ASSERT_THROWS(parseString(n_structure_single_point), afl::except::FileFormatException);

    static const uint8_t n_structure_single_star[] = {
        '*',
    };
    TS_ASSERT_THROWS(parseString(n_structure_single_star), afl::except::FileFormatException);

    static const uint8_t n_structure_trailing__[] = {
        '{', '"', 'a', '"', ':', '"', 'b', '"', '}', '#', '{', '}',
    };
    TS_ASSERT_THROWS(parseString(n_structure_trailing__), afl::except::FileFormatException);

    static const uint8_t n_structure_uescaped_LF_before_string[] = {
        '[',  92, 'u', '0', '0', '0', 'A', '"', '"', ']',
    };
    TS_ASSERT_THROWS(parseString(n_structure_uescaped_LF_before_string), afl::except::FileFormatException);

    static const uint8_t n_structure_unclosed_array[] = {
        '[', '1',
    };
    TS_ASSERT_THROWS(parseString(n_structure_unclosed_array), afl::except::FileFormatException);

    static const uint8_t n_structure_unclosed_array_partial_null[] = {
        '[', ' ', 'f', 'a', 'l', 's', 'e', ',', ' ', 'n', 'u', 'l',
    };
    TS_ASSERT_THROWS(parseString(n_structure_unclosed_array_partial_null), afl::except::FileFormatException);

    static const uint8_t n_structure_unclosed_array_unfinished_false[] = {
        '[', ' ', 't', 'r', 'u', 'e', ',', ' ', 'f', 'a', 'l', 's',
    };
    TS_ASSERT_THROWS(parseString(n_structure_unclosed_array_unfinished_false), afl::except::FileFormatException);

    static const uint8_t n_structure_unclosed_array_unfinished_true[] = {
        '[', ' ', 'f', 'a', 'l', 's', 'e', ',', ' ', 't', 'r', 'u',
    };
    TS_ASSERT_THROWS(parseString(n_structure_unclosed_array_unfinished_true), afl::except::FileFormatException);

    static const uint8_t n_structure_unclosed_object[] = {
        '{', '"', 'a', 's', 'd', '"', ':', '"', 'a', 's', 'd', '"',
    };
    TS_ASSERT_THROWS(parseString(n_structure_unclosed_object), afl::except::FileFormatException);

    static const uint8_t n_structure_unicode_identifier[] = {
        195, 165,
    };
    TS_ASSERT_THROWS(parseString(n_structure_unicode_identifier), afl::except::FileFormatException);

    static const uint8_t n_structure_whitespace_U_2060_word_joiner[] = {
        '[', 226, 129, 160, ']',
    };
    TS_ASSERT_THROWS(parseString(n_structure_whitespace_U_2060_word_joiner), afl::except::FileFormatException);

    static const uint8_t n_structure_whitespace_formfeed[] = {
        '[',  12, ']',
    };
    TS_ASSERT_THROWS(parseString(n_structure_whitespace_formfeed), afl::except::FileFormatException);

}

/** Test "ignore" cases. We have filtered them to cases we parse. */
void
TestIoJsonParser::testTestsuiteUgly()
{
    static const uint8_t i_object_key_lone_2nd_surrogate[] = {
        '{', '"',  92, 'u', 'D', 'F', 'A', 'A', '"', ':', '0', '}',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(i_object_key_lone_2nd_surrogate));

    static const uint8_t i_string_1st_surrogate_but_2nd_missing[] = {
        '[', '"',  92, 'u', 'D', 'A', 'D', 'A', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(i_string_1st_surrogate_but_2nd_missing));

    static const uint8_t i_string_1st_valid_surrogate_2nd_invalid[] = {
        '[', '"',  92, 'u', 'D', '8', '8', '8',  92, 'u', '1', '2', '3', '4', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(i_string_1st_valid_surrogate_2nd_invalid));

    static const uint8_t i_string_UTF_16_invalid_lonely_surrogate[] = {
        '[', '"',  92, 'u', 'd', '8', '0', '0', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(i_string_UTF_16_invalid_lonely_surrogate));

    static const uint8_t i_string_UTF_16_invalid_surrogate[] = {
        '[', '"',  92, 'u', 'd', '8', '0', '0', 'a', 'b', 'c', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(i_string_UTF_16_invalid_surrogate));

    static const uint8_t i_string_UTF_8_invalid_sequence[] = {
        '[', '"', 230, 151, 165, 209, 136, 250, '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(i_string_UTF_8_invalid_sequence));

    static const uint8_t i_string_incomplete_surrogate_and_escape_valid[] = {
        '[', '"',  92, 'u', 'D', '8', '0', '0',  92, 'n', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(i_string_incomplete_surrogate_and_escape_valid));

    static const uint8_t i_string_incomplete_surrogate_pair[] = {
        '[', '"',  92, 'u', 'D', 'd', '1', 'e', 'a', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(i_string_incomplete_surrogate_pair));

    static const uint8_t i_string_incomplete_surrogates_escape_valid[] = {
        '[', '"',  92, 'u', 'D', '8', '0', '0',  92, 'u', 'D', '8', '0', '0',  92, 'n', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(i_string_incomplete_surrogates_escape_valid));

    static const uint8_t i_string_inverted_surrogates_U_1D11E[] = {
        '[', '"',  92, 'u', 'D', 'd', '1', 'e',  92, 'u', 'D', '8', '3', '4', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(i_string_inverted_surrogates_U_1D11E));

    static const uint8_t i_string_lone_second_surrogate[] = {
        '[', '"',  92, 'u', 'D', 'F', 'A', 'A', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(i_string_lone_second_surrogate));

    static const uint8_t i_string_not_in_unicode_range[] = {
        '[', '"', 244, 191, 191, 191, '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(i_string_not_in_unicode_range));

    static const uint8_t i_string_truncated_utf_8[] = {
        '[', '"', 224, 255, '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(i_string_truncated_utf_8));

    static const uint8_t i_string_unicode_U_10FFFE_nonchar[] = {
        '[', '"',  92, 'u', 'D', 'B', 'F', 'F',  92, 'u', 'D', 'F', 'F', 'E', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(i_string_unicode_U_10FFFE_nonchar));

    static const uint8_t i_string_unicode_U_1FFFE_nonchar[] = {
        '[', '"',  92, 'u', 'D', '8', '3', 'F',  92, 'u', 'D', 'F', 'F', 'E', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(i_string_unicode_U_1FFFE_nonchar));

    static const uint8_t i_string_unicode_U_FDD0_nonchar[] = {
        '[', '"',  92, 'u', 'F', 'D', 'D', '0', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(i_string_unicode_U_FDD0_nonchar));

    static const uint8_t i_string_unicode_U_FFFE_nonchar[] = {
        '[', '"',  92, 'u', 'F', 'F', 'F', 'E', '"', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(i_string_unicode_U_FFFE_nonchar));

    static const uint8_t i_structure_500_nested_arrays[] = {
        '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[',
        '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[',
        '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[',
        '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[',
        '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[',
        '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[',
        '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[',
        '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[',
        '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[',
        '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[',
        '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[',
        '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[',
        '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[',
        '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[',
        '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[',
        '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[',
        '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[',
        '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[',
        '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[',
        '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[',
        '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[',
        '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[',
        '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[',
        '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[',
        '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[', '[',
        ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']',
        ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']',
        ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']',
        ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']',
        ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']',
        ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']',
        ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']',
        ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']',
        ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']',
        ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']',
        ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']',
        ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']',
        ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']',
        ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']',
        ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']',
        ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']',
        ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']',
        ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']',
        ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']',
        ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']',
        ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']',
        ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']',
        ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']',
        ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']',
        ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']', ']',
    };
    TS_ASSERT_THROWS_NOTHING(parseString(i_structure_500_nested_arrays));

}
