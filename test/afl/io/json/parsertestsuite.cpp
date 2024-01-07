/**
  *  \file test/afl/io/json/parsertestsuite.cpp
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

#include "afl/data/defaultvaluefactory.hpp"
#include "afl/data/value.hpp"
#include "afl/except/fileformatexception.hpp"
#include "afl/io/bufferedstream.hpp"
#include "afl/io/constmemorystream.hpp"
#include "afl/test/testrunner.hpp"

namespace {
    void parseString(afl::base::ConstBytes_t str)
    {
        afl::data::DefaultValueFactory factory;
        afl::io::ConstMemoryStream cms(str);
        afl::io::BufferedStream bs(cms);
        delete afl::io::json::Parser(bs, factory).parseComplete();
    }
}

/*
 *  Test "good" cases. These must all parse successfully.
 */
AFL_TEST("afl.io.json.Parser:good:y_array_arraysWithSpaces", a) {
    static const uint8_t y_array_arraysWithSpaces[] = {
        '[', '[', ']', ' ', ' ', ' ', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_array_arraysWithSpaces));
}

AFL_TEST("afl.io.json.Parser:good:y_array_empty_string", a) {
    static const uint8_t y_array_empty_string[] = {
        '[', '"', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_array_empty_string));
}

AFL_TEST("afl.io.json.Parser:good:y_array_empty", a) {
    static const uint8_t y_array_empty[] = {
        '[', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_array_empty));
}

AFL_TEST("afl.io.json.Parser:good:y_array_ending_with_newline", a) {
    static const uint8_t y_array_ending_with_newline[] = {
        '[', '"', 'a', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_array_ending_with_newline));
}

AFL_TEST("afl.io.json.Parser:good:y_array_false", a) {
    static const uint8_t y_array_false[] = {
        '[', 'f', 'a', 'l', 's', 'e', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_array_false));
}

AFL_TEST("afl.io.json.Parser:good:y_array_heterogeneous", a) {
    static const uint8_t y_array_heterogeneous[] = {
        '[', 'n', 'u', 'l', 'l', ',', ' ', '1', ',', ' ', '"', '1', '"', ',', ' ', '{', '}', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_array_heterogeneous));
}

AFL_TEST("afl.io.json.Parser:good:y_array_null", a) {
    static const uint8_t y_array_null[] = {
        '[', 'n', 'u', 'l', 'l', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_array_null));
}

AFL_TEST("afl.io.json.Parser:good:y_array_with_1_and_newline", a) {
    static const uint8_t y_array_with_1_and_newline[] = {
        '[', '1',  10, ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_array_with_1_and_newline));
}

AFL_TEST("afl.io.json.Parser:good:y_array_with_leading_space", a) {
    static const uint8_t y_array_with_leading_space[] = {
        ' ', '[', '1', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_array_with_leading_space));
}

AFL_TEST("afl.io.json.Parser:good:y_array_with_several_null", a) {
    static const uint8_t y_array_with_several_null[] = {
        '[', '1', ',', 'n', 'u', 'l', 'l', ',', 'n', 'u', 'l', 'l', ',', 'n', 'u', 'l', 'l', ',', '2', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_array_with_several_null));
}

AFL_TEST("afl.io.json.Parser:good:y_array_with_trailing_space", a) {
    static const uint8_t y_array_with_trailing_space[] = {
        '[', '2', ']', ' ',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_array_with_trailing_space));
}

AFL_TEST("afl.io.json.Parser:good:y_number", a) {
    static const uint8_t y_number[] = {
        '[', '1', '2', '3', 'e', '6', '5', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_number));
}

AFL_TEST("afl.io.json.Parser:good:y_number_0e_1", a) {
    static const uint8_t y_number_0e_1[] = {
        '[', '0', 'e', '+', '1', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_number_0e_1));
}

AFL_TEST("afl.io.json.Parser:good:y_number_0e1", a) {
    static const uint8_t y_number_0e1[] = {
        '[', '0', 'e', '1', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_number_0e1));
}

AFL_TEST("afl.io.json.Parser:good:y_number_after_space", a) {
    static const uint8_t y_number_after_space[] = {
        '[', ' ', '4', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_number_after_space));
}

AFL_TEST("afl.io.json.Parser:good:y_number_double_close_to_zero", a) {
    static const uint8_t y_number_double_close_to_zero[] = {
        '[', '-', '0', '.', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
        '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
        '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
        '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
        '0', '1', ']',  10,
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_number_double_close_to_zero));
}

AFL_TEST("afl.io.json.Parser:good:y_number_int_with_exp", a) {
    static const uint8_t y_number_int_with_exp[] = {
        '[', '2', '0', 'e', '1', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_number_int_with_exp));
}

AFL_TEST("afl.io.json.Parser:good:y_number_minus_zero", a) {
    static const uint8_t y_number_minus_zero[] = {
        '[', '-', '0', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_number_minus_zero));
}

AFL_TEST("afl.io.json.Parser:good:y_number_negative_int", a) {
    static const uint8_t y_number_negative_int[] = {
        '[', '-', '1', '2', '3', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_number_negative_int));
}

AFL_TEST("afl.io.json.Parser:good:y_number_negative_one", a) {
    static const uint8_t y_number_negative_one[] = {
        '[', '-', '1', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_number_negative_one));
}

AFL_TEST("afl.io.json.Parser:good:y_number_negative_zero", a) {
    static const uint8_t y_number_negative_zero[] = {
        '[', '-', '0', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_number_negative_zero));
}

AFL_TEST("afl.io.json.Parser:good:y_number_real_capital_e", a) {
    static const uint8_t y_number_real_capital_e[] = {
        '[', '1', 'E', '2', '2', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_number_real_capital_e));
}

AFL_TEST("afl.io.json.Parser:good:y_number_real_capital_e_neg_exp", a) {
    static const uint8_t y_number_real_capital_e_neg_exp[] = {
        '[', '1', 'E', '-', '2', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_number_real_capital_e_neg_exp));
}

AFL_TEST("afl.io.json.Parser:good:y_number_real_capital_e_pos_exp", a) {
    static const uint8_t y_number_real_capital_e_pos_exp[] = {
        '[', '1', 'E', '+', '2', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_number_real_capital_e_pos_exp));
}

AFL_TEST("afl.io.json.Parser:good:y_number_real_exponent", a) {
    static const uint8_t y_number_real_exponent[] = {
        '[', '1', '2', '3', 'e', '4', '5', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_number_real_exponent));
}

AFL_TEST("afl.io.json.Parser:good:y_number_real_fraction_exponent", a) {
    static const uint8_t y_number_real_fraction_exponent[] = {
        '[', '1', '2', '3', '.', '4', '5', '6', 'e', '7', '8', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_number_real_fraction_exponent));
}

AFL_TEST("afl.io.json.Parser:good:y_number_real_neg_exp", a) {
    static const uint8_t y_number_real_neg_exp[] = {
        '[', '1', 'e', '-', '2', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_number_real_neg_exp));
}

AFL_TEST("afl.io.json.Parser:good:y_number_real_pos_exponent", a) {
    static const uint8_t y_number_real_pos_exponent[] = {
        '[', '1', 'e', '+', '2', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_number_real_pos_exponent));
}

AFL_TEST("afl.io.json.Parser:good:y_number_simple_int", a) {
    static const uint8_t y_number_simple_int[] = {
        '[', '1', '2', '3', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_number_simple_int));
}

AFL_TEST("afl.io.json.Parser:good:y_number_simple_real", a) {
    static const uint8_t y_number_simple_real[] = {
        '[', '1', '2', '3', '.', '4', '5', '6', '7', '8', '9', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_number_simple_real));
}

AFL_TEST("afl.io.json.Parser:good:y_number_too_big_neg_int", a) {
    static const uint8_t y_number_too_big_neg_int[] = {
        '[', '-', '1', '2', '3', '1', '2', '3', '1', '2', '3', '1', '2', '3', '1', '2', '3', '1', '2', '3',
        '1', '2', '3', '1', '2', '3', '1', '2', '3', '1', '2', '3', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_number_too_big_neg_int));
}

AFL_TEST("afl.io.json.Parser:good:y_number_too_big_pos_int", a) {
    static const uint8_t y_number_too_big_pos_int[] = {
        '[', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
        '0', '0', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_number_too_big_pos_int));
}

AFL_TEST("afl.io.json.Parser:good:y_number_very_big_negative_int", a) {
    static const uint8_t y_number_very_big_negative_int[] = {
        '[', '-', '2', '3', '7', '4', '6', '2', '3', '7', '4', '6', '7', '3', '2', '7', '6', '8', '9', '4',
        '2', '7', '9', '8', '3', '2', '7', '4', '9', '8', '3', '2', '4', '2', '3', '4', '7', '9', '8', '2',
        '3', '2', '4', '6', '3', '2', '7', '8', '4', '6', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_number_very_big_negative_int));
}

AFL_TEST("afl.io.json.Parser:good:y_object", a) {
    static const uint8_t y_object[] = {
        '{', '"', 'a', 's', 'd', '"', ':', '"', 's', 'd', 'f', '"', ',', ' ', '"', 'd', 'f', 'g', '"', ':',
        '"', 'f', 'g', 'h', '"', '}',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_object));
}

AFL_TEST("afl.io.json.Parser:good:y_object_basic", a) {
    static const uint8_t y_object_basic[] = {
        '{', '"', 'a', 's', 'd', '"', ':', '"', 's', 'd', 'f', '"', '}',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_object_basic));
}

AFL_TEST("afl.io.json.Parser:good:y_object_duplicated_key", a) {
    static const uint8_t y_object_duplicated_key[] = {
        '{', '"', 'a', '"', ':', '"', 'b', '"', ',', '"', 'a', '"', ':', '"', 'c', '"', '}',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_object_duplicated_key));
}

AFL_TEST("afl.io.json.Parser:good:y_object_duplicated_key_and_value", a) {
    static const uint8_t y_object_duplicated_key_and_value[] = {
        '{', '"', 'a', '"', ':', '"', 'b', '"', ',', '"', 'a', '"', ':', '"', 'b', '"', '}',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_object_duplicated_key_and_value));
}

AFL_TEST("afl.io.json.Parser:good:y_object_empty", a) {
    static const uint8_t y_object_empty[] = {
        '{', '}',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_object_empty));
}

AFL_TEST("afl.io.json.Parser:good:y_object_empty_key", a) {
    static const uint8_t y_object_empty_key[] = {
        '{', '"', '"', ':', '0', '}',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_object_empty_key));
}

AFL_TEST("afl.io.json.Parser:good:y_object_escaped_null_in_key", a) {
    static const uint8_t y_object_escaped_null_in_key[] = {
        '{', '"', 'f', 'o', 'o',  92, 'u', '0', '0', '0', '0', 'b', 'a', 'r', '"', ':', ' ', '4', '2', '}',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_object_escaped_null_in_key));
}

AFL_TEST("afl.io.json.Parser:good:y_object_extreme_numbers", a) {
    static const uint8_t y_object_extreme_numbers[] = {
        '{', ' ', '"', 'm', 'i', 'n', '"', ':', ' ', '-', '1', '.', '0', 'e', '+', '2', '8', ',', ' ', '"',
        'm', 'a', 'x', '"', ':', ' ', '1', '.', '0', 'e', '+', '2', '8', ' ', '}',  10,
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_object_extreme_numbers));
}

AFL_TEST("afl.io.json.Parser:good:y_object_long_strings", a) {
    static const uint8_t y_object_long_strings[] = {
        '{', '"', 'x', '"', ':', '[', '{', '"', 'i', 'd', '"', ':', ' ', '"', 'x', 'x', 'x', 'x', 'x', 'x',
        'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
        'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', '"', '}', ']', ',', ' ', '"',
        'i', 'd', '"', ':', ' ', '"', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
        'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
        'x', 'x', 'x', 'x', 'x', 'x', '"', '}',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_object_long_strings));
}

AFL_TEST("afl.io.json.Parser:good:y_object_simple", a) {
    static const uint8_t y_object_simple[] = {
        '{', '"', 'a', '"', ':', '[', ']', '}',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_object_simple));
}

AFL_TEST("afl.io.json.Parser:good:y_object_string_unicode", a) {
    static const uint8_t y_object_string_unicode[] = {
        '{', '"', 't', 'i', 't', 'l', 'e', '"', ':', '"',  92, 'u', '0', '4', '1', 'f',  92, 'u', '0', '4',
        '3', 'e',  92, 'u', '0', '4', '3', 'b',  92, 'u', '0', '4', '4', '2',  92, 'u', '0', '4', '3', 'e',
         92, 'u', '0', '4', '4', '0',  92, 'u', '0', '4', '3', '0', ' ',  92, 'u', '0', '4', '1', '7',  92,
        'u', '0', '4', '3', '5',  92, 'u', '0', '4', '3', 'c',  92, 'u', '0', '4', '3', 'b',  92, 'u', '0',
        '4', '3', '5',  92, 'u', '0', '4', '3', 'a',  92, 'u', '0', '4', '3', 'e',  92, 'u', '0', '4', '3',
        'f',  92, 'u', '0', '4', '3', '0', '"', ' ', '}',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_object_string_unicode));
}

AFL_TEST("afl.io.json.Parser:good:y_object_with_newlines", a) {
    static const uint8_t y_object_with_newlines[] = {
        '{',  10, '"', 'a', '"', ':', ' ', '"', 'b', '"',  10, '}',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_object_with_newlines));
}

AFL_TEST("afl.io.json.Parser:good:y_string_1_2_3_bytes_UTF_8_sequences", a) {
    static const uint8_t y_string_1_2_3_bytes_UTF_8_sequences[] = {
        '[', '"',  92, 'u', '0', '0', '6', '0',  92, 'u', '0', '1', '2', 'a',  92, 'u', '1', '2', 'A', 'B',
        '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_1_2_3_bytes_UTF_8_sequences));
}

AFL_TEST("afl.io.json.Parser:good:y_string_UTF_16_Surrogates_U_1D11E_MUSICAL_SYMBOL_G_CLEF", a) {
    static const uint8_t y_string_UTF_16_Surrogates_U_1D11E_MUSICAL_SYMBOL_G_CLEF[] = {
        '[', '"',  92, 'u', 'D', '8', '3', '4',  92, 'u', 'D', 'd', '1', 'e', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_UTF_16_Surrogates_U_1D11E_MUSICAL_SYMBOL_G_CLEF));
}

AFL_TEST("afl.io.json.Parser:good:y_string_accepted_surrogate_pair", a) {
    static const uint8_t y_string_accepted_surrogate_pair[] = {
        '[', '"',  92, 'u', 'D', '8', '0', '1',  92, 'u', 'd', 'c', '3', '7', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_accepted_surrogate_pair));
}

AFL_TEST("afl.io.json.Parser:good:y_string_accepted_surrogate_pairs", a) {
    static const uint8_t y_string_accepted_surrogate_pairs[] = {
        '[', '"',  92, 'u', 'd', '8', '3', 'd',  92, 'u', 'd', 'e', '3', '9',  92, 'u', 'd', '8', '3', 'd',
         92, 'u', 'd', 'c', '8', 'd', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_accepted_surrogate_pairs));
}

AFL_TEST("afl.io.json.Parser:good:y_string_allowed_escapes", a) {
    static const uint8_t y_string_allowed_escapes[] = {
        '[', '"',  92, '"',  92,  92,  92, '/',  92, 'b',  92, 'f',  92, 'n',  92, 'r',  92, 't', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_allowed_escapes));
}

AFL_TEST("afl.io.json.Parser:good:y_string_backslash_and_u_escaped_zero", a) {
    static const uint8_t y_string_backslash_and_u_escaped_zero[] = {
        '[', '"',  92,  92, 'u', '0', '0', '0', '0', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_backslash_and_u_escaped_zero));
}

AFL_TEST("afl.io.json.Parser:good:y_string_backslash_doublequotes", a) {
    static const uint8_t y_string_backslash_doublequotes[] = {
        '[', '"',  92, '"', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_backslash_doublequotes));
}

AFL_TEST("afl.io.json.Parser:good:y_string_comments", a) {
    static const uint8_t y_string_comments[] = {
        '[', '"', 'a', '/', '*', 'b', '*', '/', 'c', '/', '*', 'd', '/', '/', 'e', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_comments));
}

AFL_TEST("afl.io.json.Parser:good:y_string_double_escape_a", a) {
    static const uint8_t y_string_double_escape_a[] = {
        '[', '"',  92,  92, 'a', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_double_escape_a));
}

AFL_TEST("afl.io.json.Parser:good:y_string_double_escape_n", a) {
    static const uint8_t y_string_double_escape_n[] = {
        '[', '"',  92,  92, 'n', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_double_escape_n));
}

AFL_TEST("afl.io.json.Parser:good:y_string_escaped_control_character", a) {
    static const uint8_t y_string_escaped_control_character[] = {
        '[', '"',  92, 'u', '0', '0', '1', '2', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_escaped_control_character));
}

AFL_TEST("afl.io.json.Parser:good:y_string_escaped_noncharacter", a) {
    static const uint8_t y_string_escaped_noncharacter[] = {
        '[', '"',  92, 'u', 'F', 'F', 'F', 'F', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_escaped_noncharacter));
}

AFL_TEST("afl.io.json.Parser:good:y_string_in_array", a) {
    static const uint8_t y_string_in_array[] = {
        '[', '"', 'a', 's', 'd', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_in_array));
}

AFL_TEST("afl.io.json.Parser:good:y_string_in_array_with_leading_space", a) {
    static const uint8_t y_string_in_array_with_leading_space[] = {
        '[', ' ', '"', 'a', 's', 'd', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_in_array_with_leading_space));
}

AFL_TEST("afl.io.json.Parser:good:y_string_last_surrogates_1_and_2", a) {
    static const uint8_t y_string_last_surrogates_1_and_2[] = {
        '[', '"',  92, 'u', 'D', 'B', 'F', 'F',  92, 'u', 'D', 'F', 'F', 'F', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_last_surrogates_1_and_2));
}

AFL_TEST("afl.io.json.Parser:good:y_string_newline_uescaped", a) {
    static const uint8_t y_string_newline_uescaped[] = {
        '[', '"', 'n', 'e', 'w',  92, 'u', '0', '0', 'A', '0', 'l', 'i', 'n', 'e', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_newline_uescaped));
}

AFL_TEST("afl.io.json.Parser:good:y_string_nonCharacterInUTF_8_U_10FFFF", a) {
    static const uint8_t y_string_nonCharacterInUTF_8_U_10FFFF[] = {
        '[', '"', 244, 143, 191, 191, '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_nonCharacterInUTF_8_U_10FFFF));
}

AFL_TEST("afl.io.json.Parser:good:y_string_nonCharacterInUTF_8_U_1FFFF", a) {
    static const uint8_t y_string_nonCharacterInUTF_8_U_1FFFF[] = {
        '[', '"', 240, 155, 191, 191, '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_nonCharacterInUTF_8_U_1FFFF));
}

AFL_TEST("afl.io.json.Parser:good:y_string_nonCharacterInUTF_8_U_FFFF", a) {
    static const uint8_t y_string_nonCharacterInUTF_8_U_FFFF[] = {
        '[', '"', 239, 191, 191, '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_nonCharacterInUTF_8_U_FFFF));
}

AFL_TEST("afl.io.json.Parser:good:y_string_null_escape", a) {
    static const uint8_t y_string_null_escape[] = {
        '[', '"',  92, 'u', '0', '0', '0', '0', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_null_escape));
}

AFL_TEST("afl.io.json.Parser:good:y_string_one_byte_utf_8", a) {
    static const uint8_t y_string_one_byte_utf_8[] = {
        '[', '"',  92, 'u', '0', '0', '2', 'c', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_one_byte_utf_8));
}

AFL_TEST("afl.io.json.Parser:good:y_string_pi", a) {
    static const uint8_t y_string_pi[] = {
        '[', '"', 207, 128, '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_pi));
}

AFL_TEST("afl.io.json.Parser:good:y_string_simple_ascii", a) {
    static const uint8_t y_string_simple_ascii[] = {
        '[', '"', 'a', 's', 'd', ' ', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_simple_ascii));
}

AFL_TEST("afl.io.json.Parser:good:y_string_space", a) {
    static const uint8_t y_string_space[] = {
        '"', ' ', '"',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_space));
}

AFL_TEST("afl.io.json.Parser:good:y_string_three_byte_utf_8", a) {
    static const uint8_t y_string_three_byte_utf_8[] = {
        '[', '"',  92, 'u', '0', '8', '2', '1', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_three_byte_utf_8));
}

AFL_TEST("afl.io.json.Parser:good:y_string_two_byte_utf_8", a) {
    static const uint8_t y_string_two_byte_utf_8[] = {
        '[', '"',  92, 'u', '0', '1', '2', '3', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_two_byte_utf_8));
}

AFL_TEST("afl.io.json.Parser:good:y_string_u_2028_line_sep", a) {
    static const uint8_t y_string_u_2028_line_sep[] = {
        '[', '"', 226, 128, 168, '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_u_2028_line_sep));
}

AFL_TEST("afl.io.json.Parser:good:y_string_u_2029_par_sep", a) {
    static const uint8_t y_string_u_2029_par_sep[] = {
        '[', '"', 226, 128, 169, '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_u_2029_par_sep));
}

AFL_TEST("afl.io.json.Parser:good:y_string_uEscape", a) {
    static const uint8_t y_string_uEscape[] = {
        '[', '"',  92, 'u', '0', '0', '6', '1',  92, 'u', '3', '0', 'a', 'f',  92, 'u', '3', '0', 'E', 'A',
         92, 'u', '3', '0', 'b', '9', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_uEscape));
}

AFL_TEST("afl.io.json.Parser:good:y_string_unescaped_char_delete", a) {
    static const uint8_t y_string_unescaped_char_delete[] = {
        '[', '"', 127, '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_unescaped_char_delete));
}

AFL_TEST("afl.io.json.Parser:good:y_string_unicode", a) {
    static const uint8_t y_string_unicode[] = {
        '[', '"',  92, 'u', 'A', '6', '6', 'D', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_unicode));
}

AFL_TEST("afl.io.json.Parser:good:y_string_unicodeEscapedBackslash", a) {
    static const uint8_t y_string_unicodeEscapedBackslash[] = {
        '[', '"',  92, 'u', '0', '0', '5', 'C', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_unicodeEscapedBackslash));
}

AFL_TEST("afl.io.json.Parser:good:y_string_unicode_2", a) {
    static const uint8_t y_string_unicode_2[] = {
        '[', '"', 226, 141, 130, 227, 136, 180, 226, 141, 130, '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_unicode_2));
}

AFL_TEST("afl.io.json.Parser:good:y_string_unicode_U_200B_ZERO_WIDTH_SPACE", a) {
    static const uint8_t y_string_unicode_U_200B_ZERO_WIDTH_SPACE[] = {
        '[', '"',  92, 'u', '2', '0', '0', 'B', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_unicode_U_200B_ZERO_WIDTH_SPACE));
}

AFL_TEST("afl.io.json.Parser:good:y_string_unicode_U_2064_invisible_plus", a) {
    static const uint8_t y_string_unicode_U_2064_invisible_plus[] = {
        '[', '"',  92, 'u', '2', '0', '6', '4', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_unicode_U_2064_invisible_plus));
}

AFL_TEST("afl.io.json.Parser:good:y_string_unicode_escaped_double_quote", a) {
    static const uint8_t y_string_unicode_escaped_double_quote[] = {
        '[', '"',  92, 'u', '0', '0', '2', '2', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_unicode_escaped_double_quote));
}

AFL_TEST("afl.io.json.Parser:good:y_string_utf8", a) {
    static const uint8_t y_string_utf8[] = {
        '[', '"', 226, 130, 172, 240, 157, 132, 158, '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_utf8));
}

AFL_TEST("afl.io.json.Parser:good:y_string_with_del_character", a) {
    static const uint8_t y_string_with_del_character[] = {
        '[', '"', 'a', 127, 'a', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_string_with_del_character));
}

AFL_TEST("afl.io.json.Parser:good:y_structure_lonely_false", a) {
    static const uint8_t y_structure_lonely_false[] = {
        'f', 'a', 'l', 's', 'e',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_structure_lonely_false));
}

AFL_TEST("afl.io.json.Parser:good:y_structure_lonely_int", a) {
    static const uint8_t y_structure_lonely_int[] = {
        '4', '2',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_structure_lonely_int));
}

AFL_TEST("afl.io.json.Parser:good:y_structure_lonely_negative_real", a) {
    static const uint8_t y_structure_lonely_negative_real[] = {
        '-', '0', '.', '1',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_structure_lonely_negative_real));
}

AFL_TEST("afl.io.json.Parser:good:y_structure_lonely_null", a) {
    static const uint8_t y_structure_lonely_null[] = {
        'n', 'u', 'l', 'l',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_structure_lonely_null));
}

AFL_TEST("afl.io.json.Parser:good:y_structure_lonely_string", a) {
    static const uint8_t y_structure_lonely_string[] = {
        '"', 'a', 's', 'd', '"',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_structure_lonely_string));
}

AFL_TEST("afl.io.json.Parser:good:y_structure_lonely_true", a) {
    static const uint8_t y_structure_lonely_true[] = {
        't', 'r', 'u', 'e',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_structure_lonely_true));
}

AFL_TEST("afl.io.json.Parser:good:y_structure_string_empty", a) {
    static const uint8_t y_structure_string_empty[] = {
        '"', '"',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_structure_string_empty));
}

AFL_TEST("afl.io.json.Parser:good:y_structure_trailing_newline", a) {
    static const uint8_t y_structure_trailing_newline[] = {
        '[', '"', 'a', '"', ']',  10,
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_structure_trailing_newline));
}

AFL_TEST("afl.io.json.Parser:good:y_structure_true_in_array", a) {
    static const uint8_t y_structure_true_in_array[] = {
        '[', 't', 'r', 'u', 'e', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_structure_true_in_array));
}

AFL_TEST("afl.io.json.Parser:good:y_structure_whitespace_array", a) {
    static const uint8_t y_structure_whitespace_array[] = {
        ' ', '[', ']', ' ',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(y_structure_whitespace_array));
}


/*
 *  Test "bad" cases. These must all fail.
 */
AFL_TEST("afl.io.json.Parser:bad:n_array_1_true_without_comma", a) {
    static const uint8_t n_array_1_true_without_comma[] = {
        '[', '1', ' ', 't', 'r', 'u', 'e', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_array_1_true_without_comma), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_array_a_invalid_utf8", a) {
    static const uint8_t n_array_a_invalid_utf8[] = {
        '[', 'a', 229, ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_array_a_invalid_utf8), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_array_colon_instead_of_comma", a) {
    static const uint8_t n_array_colon_instead_of_comma[] = {
        '[', '"', '"', ':', ' ', '1', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_array_colon_instead_of_comma), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_array_comma_after_close", a) {
    static const uint8_t n_array_comma_after_close[] = {
        '[', '"', '"', ']', ',',
    };
    AFL_CHECK_THROWS(a, parseString(n_array_comma_after_close), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_array_comma_and_number", a) {
    static const uint8_t n_array_comma_and_number[] = {
        '[', ',', '1', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_array_comma_and_number), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_array_double_comma", a) {
    static const uint8_t n_array_double_comma[] = {
        '[', '1', ',', ',', '2', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_array_double_comma), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_array_double_extra_comma", a) {
    static const uint8_t n_array_double_extra_comma[] = {
        '[', '"', 'x', '"', ',', ',', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_array_double_extra_comma), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_array_extra_close", a) {
    static const uint8_t n_array_extra_close[] = {
        '[', '"', 'x', '"', ']', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_array_extra_close), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_array_extra_comma", a) {
    static const uint8_t n_array_extra_comma[] = {
        '[', '"', '"', ',', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_array_extra_comma), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_array_incomplete", a) {
    static const uint8_t n_array_incomplete[] = {
        '[', '"', 'x', '"',
    };
    AFL_CHECK_THROWS(a, parseString(n_array_incomplete), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_array_incomplete_invalid_value", a) {
    static const uint8_t n_array_incomplete_invalid_value[] = {
        '[', 'x',
    };
    AFL_CHECK_THROWS(a, parseString(n_array_incomplete_invalid_value), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_array_inner_array_no_comma", a) {
    static const uint8_t n_array_inner_array_no_comma[] = {
        '[', '3', '[', '4', ']', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_array_inner_array_no_comma), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_array_invalid_utf8", a) {
    static const uint8_t n_array_invalid_utf8[] = {
        '[', 255, ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_array_invalid_utf8), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_array_items_separated_by_semicolon", a) {
    static const uint8_t n_array_items_separated_by_semicolon[] = {
        '[', '1', ':', '2', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_array_items_separated_by_semicolon), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_array_just_comma", a) {
    static const uint8_t n_array_just_comma[] = {
        '[', ',', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_array_just_comma), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_array_just_minus", a) {
    static const uint8_t n_array_just_minus[] = {
        '[', '-', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_array_just_minus), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_array_missing_value", a) {
    static const uint8_t n_array_missing_value[] = {
        '[', ' ', ' ', ' ', ',', ' ', '"', '"', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_array_missing_value), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_array_newlines_unclosed", a) {
    static const uint8_t n_array_newlines_unclosed[] = {
        '[', '"', 'a', '"', ',',  10, '4',  10, ',', '1', ',',
    };
    AFL_CHECK_THROWS(a, parseString(n_array_newlines_unclosed), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_array_number_and_comma", a) {
    static const uint8_t n_array_number_and_comma[] = {
        '[', '1', ',', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_array_number_and_comma), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_array_number_and_several_commas", a) {
    static const uint8_t n_array_number_and_several_commas[] = {
        '[', '1', ',', ',', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_array_number_and_several_commas), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_array_spaces_vertical_tab_formfeed", a) {
    static const uint8_t n_array_spaces_vertical_tab_formfeed[] = {
        '[', '"',  11, 'a', '"',  92, 'f', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_array_spaces_vertical_tab_formfeed), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_array_star_inside", a) {
    static const uint8_t n_array_star_inside[] = {
        '[', '*', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_array_star_inside), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_array_unclosed", a) {
    static const uint8_t n_array_unclosed[] = {
        '[', '"', '"',
    };
    AFL_CHECK_THROWS(a, parseString(n_array_unclosed), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_array_unclosed_trailing_comma", a) {
    static const uint8_t n_array_unclosed_trailing_comma[] = {
        '[', '1', ',',
    };
    AFL_CHECK_THROWS(a, parseString(n_array_unclosed_trailing_comma), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_array_unclosed_with_new_lines", a) {
    static const uint8_t n_array_unclosed_with_new_lines[] = {
        '[', '1', ',',  10, '1',  10, ',', '1',
    };
    AFL_CHECK_THROWS(a, parseString(n_array_unclosed_with_new_lines), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_array_unclosed_with_object_inside", a) {
    static const uint8_t n_array_unclosed_with_object_inside[] = {
        '[', '{', '}',
    };
    AFL_CHECK_THROWS(a, parseString(n_array_unclosed_with_object_inside), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_incomplete_false", a) {
    static const uint8_t n_incomplete_false[] = {
        '[', 'f', 'a', 'l', 's', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_incomplete_false), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_incomplete_null", a) {
    static const uint8_t n_incomplete_null[] = {
        '[', 'n', 'u', 'l', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_incomplete_null), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_incomplete_true", a) {
    static const uint8_t n_incomplete_true[] = {
        '[', 't', 'r', 'u', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_incomplete_true), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number__", a) {
    static const uint8_t n_number__[] = {
        '[', '+', '+', '1', '2', '3', '4', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number__), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number__1", a) {
    static const uint8_t n_number__1[] = {
        '[', '+', '1', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number__1), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number__Inf", a) {
    static const uint8_t n_number__Inf[] = {
        '[', '+', 'I', 'n', 'f', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number__Inf), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number__1_0_", a) {
    static const uint8_t n_number__1_0_[] = {
        '[', '-', '1', '.', '0', '.', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number__1_0_), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number__NaN", a) {
    static const uint8_t n_number__NaN[] = {
        '[', '-', 'N', 'a', 'N', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number__NaN), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number__1_1", a) {
    static const uint8_t n_number__1_1[] = {
        '[', '.', '-', '1', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number__1_1), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number__2e_3", a) {
    static const uint8_t n_number__2e_3[] = {
        '[', '.', '2', 'e', '-', '3', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number__2e_3), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_0_1_2", a) {
    static const uint8_t n_number_0_1_2[] = {
        '[', '0', '.', '1', '.', '2', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_0_1_2), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_0_3e_", a) {
    static const uint8_t n_number_0_3e_[] = {
        '[', '0', '.', '3', 'e', '+', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_0_3e_), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_0_3e", a) {
    static const uint8_t n_number_0_3e[] = {
        '[', '0', '.', '3', 'e', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_0_3e), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_0_capital_E_", a) {
    static const uint8_t n_number_0_capital_E_[] = {
        '[', '0', 'E', '+', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_0_capital_E_), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_0_capital_E", a) {
    static const uint8_t n_number_0_capital_E[] = {
        '[', '0', 'E', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_0_capital_E), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_0e_", a) {
    static const uint8_t n_number_0e_[] = {
        '[', '0', 'e', '+', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_0e_), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_0e", a) {
    static const uint8_t n_number_0e[] = {
        '[', '0', 'e', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_0e), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_1_0e_", a) {
    static const uint8_t n_number_1_0e_[] = {
        '[', '1', '.', '0', 'e', '+', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_1_0e_), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_1_0e__1", a) {
    static const uint8_t n_number_1_0e__1[] = {
        '[', '1', '.', '0', 'e', '-', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_1_0e__1), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_1_0e", a) {
    static const uint8_t n_number_1_0e[] = {
        '[', '1', '.', '0', 'e', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_1_0e), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_1_000", a) {
    static const uint8_t n_number_1_000[] = {
        '[', '1', ' ', '0', '0', '0', '.', '0', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_1_000), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_1eE2", a) {
    static const uint8_t n_number_1eE2[] = {
        '[', '1', 'e', 'E', '2', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_1eE2), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_9_e_", a) {
    static const uint8_t n_number_9_e_[] = {
        '[', '9', '.', 'e', '+', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_9_e_), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_Inf", a) {
    static const uint8_t n_number_Inf[] = {
        '[', 'I', 'n', 'f', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_Inf), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_NaN", a) {
    static const uint8_t n_number_NaN[] = {
        '[', 'N', 'a', 'N', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_NaN), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_U_FF11_fullwidth_digit_one", a) {
    static const uint8_t n_number_U_FF11_fullwidth_digit_one[] = {
        '[', 239, 188, 145, ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_U_FF11_fullwidth_digit_one), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_expression", a) {
    static const uint8_t n_number_expression[] = {
        '[', '1', '+', '2', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_expression), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_hex_1_digit", a) {
    static const uint8_t n_number_hex_1_digit[] = {
        '[', '0', 'x', '1', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_hex_1_digit), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_hex_2_digits", a) {
    static const uint8_t n_number_hex_2_digits[] = {
        '[', '0', 'x', '4', '2', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_hex_2_digits), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_infinity", a) {
    static const uint8_t n_number_infinity[] = {
        '[', 'I', 'n', 'f', 'i', 'n', 'i', 't', 'y', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_infinity), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_invalid_", a) {
    static const uint8_t n_number_invalid_[] = {
        '[', '0', 'e', '+', '-', '1', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_invalid_), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_invalid_negative_real", a) {
    static const uint8_t n_number_invalid_negative_real[] = {
        '[', '-', '1', '2', '3', '.', '1', '2', '3', 'f', 'o', 'o', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_invalid_negative_real), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_invalid_utf_8_in_bigger_int", a) {
    static const uint8_t n_number_invalid_utf_8_in_bigger_int[] = {
        '[', '1', '2', '3', 229, ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_invalid_utf_8_in_bigger_int), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_invalid_utf_8_in_exponent", a) {
    static const uint8_t n_number_invalid_utf_8_in_exponent[] = {
        '[', '1', 'e', '1', 229, ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_invalid_utf_8_in_exponent), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_invalid_utf_8_in_int", a) {
    static const uint8_t n_number_invalid_utf_8_in_int[] = {
        '[', '0', 229, ']',  10,
    };
    AFL_CHECK_THROWS(a, parseString(n_number_invalid_utf_8_in_int), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_minus_infinity", a) {
    static const uint8_t n_number_minus_infinity[] = {
        '[', '-', 'I', 'n', 'f', 'i', 'n', 'i', 't', 'y', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_minus_infinity), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_minus_sign_with_trailing_garbage", a) {
    static const uint8_t n_number_minus_sign_with_trailing_garbage[] = {
        '[', '-', 'f', 'o', 'o', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_minus_sign_with_trailing_garbage), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_minus_space_1", a) {
    static const uint8_t n_number_minus_space_1[] = {
        '[', '-', ' ', '1', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_minus_space_1), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_neg_with_garbage_at_end", a) {
    static const uint8_t n_number_neg_with_garbage_at_end[] = {
        '[', '-', '1', 'x', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_neg_with_garbage_at_end), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_real_garbage_after_e", a) {
    static const uint8_t n_number_real_garbage_after_e[] = {
        '[', '1', 'e', 'a', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_real_garbage_after_e), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_real_with_invalid_utf8_after_e", a) {
    static const uint8_t n_number_real_with_invalid_utf8_after_e[] = {
        '[', '1', 'e', 229, ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_real_with_invalid_utf8_after_e), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_starting_with_dot", a) {
    static const uint8_t n_number_starting_with_dot[] = {
        '[', '.', '1', '2', '3', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_starting_with_dot), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_then_00", a) {
    static const uint8_t n_number_then_00[] = {
        '1',   0,
    };
    AFL_CHECK_THROWS(a, parseString(n_number_then_00), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_with_alpha", a) {
    static const uint8_t n_number_with_alpha[] = {
        '[', '1', '.', '2', 'a', '-', '3', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_with_alpha), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_number_with_alpha_char", a) {
    static const uint8_t n_number_with_alpha_char[] = {
        '[', '1', '.', '8', '0', '1', '1', '6', '7', '0', '0', '3', '3', '3', '7', '6', '5', '1', '4', 'H',
        '-', '3', '0', '8', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_number_with_alpha_char), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_object_bad_value", a) {
    static const uint8_t n_object_bad_value[] = {
        '[', '"', 'x', '"', ',', ' ', 't', 'r', 'u', 't', 'h', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_object_bad_value), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_object_bracket_key", a) {
    static const uint8_t n_object_bracket_key[] = {
        '{', '[', ':', ' ', '"', 'x', '"', '}',  10,
    };
    AFL_CHECK_THROWS(a, parseString(n_object_bracket_key), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_object_comma_instead_of_colon", a) {
    static const uint8_t n_object_comma_instead_of_colon[] = {
        '{', '"', 'x', '"', ',', ' ', 'n', 'u', 'l', 'l', '}',
    };
    AFL_CHECK_THROWS(a, parseString(n_object_comma_instead_of_colon), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_object_double_colon", a) {
    static const uint8_t n_object_double_colon[] = {
        '{', '"', 'x', '"', ':', ':', '"', 'b', '"', '}',
    };
    AFL_CHECK_THROWS(a, parseString(n_object_double_colon), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_object_emoji", a) {
    static const uint8_t n_object_emoji[] = {
        '{', 240, 159, 135, 168, 240, 159, 135, 173, '}',
    };
    AFL_CHECK_THROWS(a, parseString(n_object_emoji), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_object_garbage_at_end", a) {
    static const uint8_t n_object_garbage_at_end[] = {
        '{', '"', 'a', '"', ':', '"', 'a', '"', ' ', '1', '2', '3', '}',
    };
    AFL_CHECK_THROWS(a, parseString(n_object_garbage_at_end), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_object_key_with_single_quotes", a) {
    static const uint8_t n_object_key_with_single_quotes[] = {
        '{', 'k', 'e', 'y', ':', ' ',  39, 'v', 'a', 'l', 'u', 'e',  39, '}',
    };
    AFL_CHECK_THROWS(a, parseString(n_object_key_with_single_quotes), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_object_missing_colon", a) {
    static const uint8_t n_object_missing_colon[] = {
        '{', '"', 'a', '"', ' ', 'b', '}',
    };
    AFL_CHECK_THROWS(a, parseString(n_object_missing_colon), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_object_missing_key", a) {
    static const uint8_t n_object_missing_key[] = {
        '{', ':', '"', 'b', '"', '}',
    };
    AFL_CHECK_THROWS(a, parseString(n_object_missing_key), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_object_missing_semicolon", a) {
    static const uint8_t n_object_missing_semicolon[] = {
        '{', '"', 'a', '"', ' ', '"', 'b', '"', '}',
    };
    AFL_CHECK_THROWS(a, parseString(n_object_missing_semicolon), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_object_missing_value", a) {
    static const uint8_t n_object_missing_value[] = {
        '{', '"', 'a', '"', ':',
    };
    AFL_CHECK_THROWS(a, parseString(n_object_missing_value), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_object_no_colon", a) {
    static const uint8_t n_object_no_colon[] = {
        '{', '"', 'a', '"',
    };
    AFL_CHECK_THROWS(a, parseString(n_object_no_colon), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_object_non_string_key", a) {
    static const uint8_t n_object_non_string_key[] = {
        '{', '1', ':', '1', '}',
    };
    AFL_CHECK_THROWS(a, parseString(n_object_non_string_key), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_object_non_string_key_but_huge_number_instead", a) {
    static const uint8_t n_object_non_string_key_but_huge_number_instead[] = {
        '{', '9', '9', '9', '9', 'E', '9', '9', '9', '9', ':', '1', '}',
    };
    AFL_CHECK_THROWS(a, parseString(n_object_non_string_key_but_huge_number_instead), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_object_pi_in_key_and_trailing_comma", a) {
    static const uint8_t n_object_pi_in_key_and_trailing_comma[] = {
        '{', '"', 185, '"', ':', '"', '0', '"', ',', '}',
    };
    AFL_CHECK_THROWS(a, parseString(n_object_pi_in_key_and_trailing_comma), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_object_repeated_null_null", a) {
    static const uint8_t n_object_repeated_null_null[] = {
        '{', 'n', 'u', 'l', 'l', ':', 'n', 'u', 'l', 'l', ',', 'n', 'u', 'l', 'l', ':', 'n', 'u', 'l', 'l',
        '}',
    };
    AFL_CHECK_THROWS(a, parseString(n_object_repeated_null_null), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_object_several_trailing_commas", a) {
    static const uint8_t n_object_several_trailing_commas[] = {
        '{', '"', 'i', 'd', '"', ':', '0', ',', ',', ',', ',', ',', '}',
    };
    AFL_CHECK_THROWS(a, parseString(n_object_several_trailing_commas), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_object_single_quote", a) {
    static const uint8_t n_object_single_quote[] = {
        '{',  39, 'a',  39, ':', '0', '}',
    };
    AFL_CHECK_THROWS(a, parseString(n_object_single_quote), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_object_trailing_comma", a) {
    static const uint8_t n_object_trailing_comma[] = {
        '{', '"', 'i', 'd', '"', ':', '0', ',', '}',
    };
    AFL_CHECK_THROWS(a, parseString(n_object_trailing_comma), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_object_trailing_comment", a) {
    static const uint8_t n_object_trailing_comment[] = {
        '{', '"', 'a', '"', ':', '"', 'b', '"', '}', '/', '*', '*', '/',
    };
    AFL_CHECK_THROWS(a, parseString(n_object_trailing_comment), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_object_trailing_comment_open", a) {
    static const uint8_t n_object_trailing_comment_open[] = {
        '{', '"', 'a', '"', ':', '"', 'b', '"', '}', '/', '*', '*', '/', '/',
    };
    AFL_CHECK_THROWS(a, parseString(n_object_trailing_comment_open), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_object_trailing_comment_slash_open", a) {
    static const uint8_t n_object_trailing_comment_slash_open[] = {
        '{', '"', 'a', '"', ':', '"', 'b', '"', '}', '/', '/',
    };
    AFL_CHECK_THROWS(a, parseString(n_object_trailing_comment_slash_open), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_object_trailing_comment_slash_open_incomplete", a) {
    static const uint8_t n_object_trailing_comment_slash_open_incomplete[] = {
        '{', '"', 'a', '"', ':', '"', 'b', '"', '}', '/',
    };
    AFL_CHECK_THROWS(a, parseString(n_object_trailing_comment_slash_open_incomplete), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_object_two_commas_in_a_row", a) {
    static const uint8_t n_object_two_commas_in_a_row[] = {
        '{', '"', 'a', '"', ':', '"', 'b', '"', ',', ',', '"', 'c', '"', ':', '"', 'd', '"', '}',
    };
    AFL_CHECK_THROWS(a, parseString(n_object_two_commas_in_a_row), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_object_unquoted_key", a) {
    static const uint8_t n_object_unquoted_key[] = {
        '{', 'a', ':', ' ', '"', 'b', '"', '}',
    };
    AFL_CHECK_THROWS(a, parseString(n_object_unquoted_key), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_object_unterminated_value", a) {
    static const uint8_t n_object_unterminated_value[] = {
        '{', '"', 'a', '"', ':', '"', 'a',
    };
    AFL_CHECK_THROWS(a, parseString(n_object_unterminated_value), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_object_with_single_string", a) {
    static const uint8_t n_object_with_single_string[] = {
        '{', ' ', '"', 'f', 'o', 'o', '"', ' ', ':', ' ', '"', 'b', 'a', 'r', '"', ',', ' ', '"', 'a', '"',
        ' ', '}',
    };
    AFL_CHECK_THROWS(a, parseString(n_object_with_single_string), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_object_with_trailing_garbage", a) {
    static const uint8_t n_object_with_trailing_garbage[] = {
        '{', '"', 'a', '"', ':', '"', 'b', '"', '}', '#',
    };
    AFL_CHECK_THROWS(a, parseString(n_object_with_trailing_garbage), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_single_space", a) {
    static const uint8_t n_single_space[] = {
        ' ',
    };
    AFL_CHECK_THROWS(a, parseString(n_single_space), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_string_1_surrogate_then_escape_u", a) {
    static const uint8_t n_string_1_surrogate_then_escape_u[] = {
        '[', '"',  92, 'u', 'D', '8', '0', '0',  92, 'u', '"', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_string_1_surrogate_then_escape_u), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_string_1_surrogate_then_escape_u1", a) {
    static const uint8_t n_string_1_surrogate_then_escape_u1[] = {
        '[', '"',  92, 'u', 'D', '8', '0', '0',  92, 'u', '1', '"', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_string_1_surrogate_then_escape_u1), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_string_1_surrogate_then_escape_u1x", a) {
    static const uint8_t n_string_1_surrogate_then_escape_u1x[] = {
        '[', '"',  92, 'u', 'D', '8', '0', '0',  92, 'u', '1', 'x', '"', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_string_1_surrogate_then_escape_u1x), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_string_1_surrogate_then_escape", a) {
    static const uint8_t n_string_1_surrogate_then_escape[] = {
        '[', '"',  92, 'u', 'D', '8', '0', '0',  92, '"', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_string_1_surrogate_then_escape), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_string_UTF_16_incomplete_surrogate", a) {
    static const uint8_t n_string_UTF_16_incomplete_surrogate[] = {
        '[', '"',  92, 'u', 'D', '8', '3', '4',  92, 'u', 'D', 'd', '"', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_string_UTF_16_incomplete_surrogate), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_string_accentuated_char_no_quotes", a) {
    static const uint8_t n_string_accentuated_char_no_quotes[] = {
        '[', 195, 169, ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_string_accentuated_char_no_quotes), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_string_backslash_00", a) {
    static const uint8_t n_string_backslash_00[] = {
        '[', '"',  92,   0, '"', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_string_backslash_00), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_string_escape_x", a) {
    static const uint8_t n_string_escape_x[] = {
        '[', '"',  92, 'x', '0', '0', '"', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_string_escape_x), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_string_escaped_backslash_bad", a) {
    static const uint8_t n_string_escaped_backslash_bad[] = {
        '[', '"',  92,  92,  92, '"', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_string_escaped_backslash_bad), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_string_escaped_ctrl_char_tab", a) {
    static const uint8_t n_string_escaped_ctrl_char_tab[] = {
        '[', '"',  92,   9, '"', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_string_escaped_ctrl_char_tab), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_string_escaped_emoji", a) {
    static const uint8_t n_string_escaped_emoji[] = {
        '[', '"',  92, 240, 159, 140, 128, '"', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_string_escaped_emoji), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_string_incomplete_escape", a) {
    static const uint8_t n_string_incomplete_escape[] = {
        '[', '"',  92, '"', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_string_incomplete_escape), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_string_incomplete_escaped_character", a) {
    static const uint8_t n_string_incomplete_escaped_character[] = {
        '[', '"',  92, 'u', '0', '0', 'A', '"', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_string_incomplete_escaped_character), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_string_incomplete_surrogate_escape_invalid", a) {
    static const uint8_t n_string_incomplete_surrogate_escape_invalid[] = {
        '[', '"',  92, 'u', 'D', '8', '0', '0',  92, 'u', 'D', '8', '0', '0',  92, 'x', '"', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_string_incomplete_surrogate_escape_invalid), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_string_invalid_utf_8_in_escape", a) {
    static const uint8_t n_string_invalid_utf_8_in_escape[] = {
        '[', '"',  92, 'u', 229, '"', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_string_invalid_utf_8_in_escape), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_string_invalid_backslash_esc", a) {
    static const uint8_t n_string_invalid_backslash_esc[] = {
        '[', '"',  92, 'a', '"', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_string_invalid_backslash_esc), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_string_invalid_unicode_escape", a) {
    static const uint8_t n_string_invalid_unicode_escape[] = {
        '[', '"',  92, 'u', 'q', 'q', 'q', 'q', '"', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_string_invalid_unicode_escape), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_string_invalid_utf8_after_escape", a) {
    static const uint8_t n_string_invalid_utf8_after_escape[] = {
        '[', '"',  92, 229, '"', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_string_invalid_utf8_after_escape), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_string_leading_uescaped_thinspace", a) {
    static const uint8_t n_string_leading_uescaped_thinspace[] = {
        '[',  92, 'u', '0', '0', '2', '0', '"', 'a', 's', 'd', '"', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_string_leading_uescaped_thinspace), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_string_no_quotes_with_bad_escape", a) {
    static const uint8_t n_string_no_quotes_with_bad_escape[] = {
        '[',  92, 'n', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_string_no_quotes_with_bad_escape), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_string_single_doublequote", a) {
    static const uint8_t n_string_single_doublequote[] = {
        '"',
    };
    AFL_CHECK_THROWS(a, parseString(n_string_single_doublequote), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_string_single_quote", a) {
    static const uint8_t n_string_single_quote[] = {
        '[',  39, 's', 'i', 'n', 'g', 'l', 'e', ' ', 'q', 'u', 'o', 't', 'e',  39, ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_string_single_quote), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_string_single_string_no_double_quotes", a) {
    static const uint8_t n_string_single_string_no_double_quotes[] = {
        'a', 'b', 'c',
    };
    AFL_CHECK_THROWS(a, parseString(n_string_single_string_no_double_quotes), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_string_start_escape_unclosed", a) {
    static const uint8_t n_string_start_escape_unclosed[] = {
        '[', '"',  92,
    };
    AFL_CHECK_THROWS(a, parseString(n_string_start_escape_unclosed), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_string_unescaped_crtl_char", a) {
    static const uint8_t n_string_unescaped_crtl_char[] = {
        '[', '"', 'a',   0, 'a', '"', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_string_unescaped_crtl_char), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_string_unescaped_newline", a) {
    static const uint8_t n_string_unescaped_newline[] = {
        '[', '"', 'n', 'e', 'w',  10, 'l', 'i', 'n', 'e', '"', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_string_unescaped_newline), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_string_unescaped_tab", a) {
    static const uint8_t n_string_unescaped_tab[] = {
        '[', '"',   9, '"', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_string_unescaped_tab), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_string_unicode_CapitalU", a) {
    static const uint8_t n_string_unicode_CapitalU[] = {
        '"',  92, 'U', 'A', '6', '6', 'D', '"',
    };
    AFL_CHECK_THROWS(a, parseString(n_string_unicode_CapitalU), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_string_with_trailing_garbage", a) {
    static const uint8_t n_string_with_trailing_garbage[] = {
        '"', '"', 'x',
    };
    AFL_CHECK_THROWS(a, parseString(n_string_with_trailing_garbage), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure__", a) {
    static const uint8_t n_structure__[] = {
        '<', '.', '>',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure__), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure__null_", a) {
    static const uint8_t n_structure__null_[] = {
        '[', '<', 'n', 'u', 'l', 'l', '>', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure__null_), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_U_2060_word_joined", a) {
    static const uint8_t n_structure_U_2060_word_joined[] = {
        '[', 226, 129, 160, ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_U_2060_word_joined), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_UTF8_BOM_no_data", a) {
    static const uint8_t n_structure_UTF8_BOM_no_data[] = {
        239, 187, 191,
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_UTF8_BOM_no_data), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_array_trailing_garbage", a) {
    static const uint8_t n_structure_array_trailing_garbage[] = {
        '[', '1', ']', 'x',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_array_trailing_garbage), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_array_with_extra_array_close", a) {
    static const uint8_t n_structure_array_with_extra_array_close[] = {
        '[', '1', ']', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_array_with_extra_array_close), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_array_with_unclosed_string", a) {
    static const uint8_t n_structure_array_with_unclosed_string[] = {
        '[', '"', 'a', 's', 'd', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_array_with_unclosed_string), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_ascii_unicode_identifier", a) {
    static const uint8_t n_structure_ascii_unicode_identifier[] = {
        'a', 195, 165,
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_ascii_unicode_identifier), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_capitalized_True", a) {
    static const uint8_t n_structure_capitalized_True[] = {
        '[', 'T', 'r', 'u', 'e', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_capitalized_True), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_close_unopened_array", a) {
    static const uint8_t n_structure_close_unopened_array[] = {
        '1', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_close_unopened_array), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_comma_instead_of_closing_brace", a) {
    static const uint8_t n_structure_comma_instead_of_closing_brace[] = {
        '{', '"', 'x', '"', ':', ' ', 't', 'r', 'u', 'e', ',',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_comma_instead_of_closing_brace), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_double_array", a) {
    static const uint8_t n_structure_double_array[] = {
        '[', ']', '[', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_double_array), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_end_array", a) {
    static const uint8_t n_structure_end_array[] = {
        ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_end_array), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_incomplete_UTF8_BOM", a) {
    static const uint8_t n_structure_incomplete_UTF8_BOM[] = {
        239, 187, '{', '}',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_incomplete_UTF8_BOM), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_lone_invalid_utf_8", a) {
    static const uint8_t n_structure_lone_invalid_utf_8[] = {
        229,
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_lone_invalid_utf_8), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_lone_open_bracket", a) {
    static const uint8_t n_structure_lone_open_bracket[] = {
        '[',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_lone_open_bracket), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_null_byte_outside_string", a) {
    static const uint8_t n_structure_null_byte_outside_string[] = {
        '[',   0, ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_null_byte_outside_string), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_number_with_trailing_garbage", a) {
    static const uint8_t n_structure_number_with_trailing_garbage[] = {
        '2', '@',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_number_with_trailing_garbage), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_object_followed_by_closing_object", a) {
    static const uint8_t n_structure_object_followed_by_closing_object[] = {
        '{', '}', '}',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_object_followed_by_closing_object), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_object_unclosed_no_value", a) {
    static const uint8_t n_structure_object_unclosed_no_value[] = {
        '{', '"', '"', ':',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_object_unclosed_no_value), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_object_with_comment", a) {
    static const uint8_t n_structure_object_with_comment[] = {
        '{', '"', 'a', '"', ':', '/', '*', 'c', 'o', 'm', 'm', 'e', 'n', 't', '*', '/', '"', 'b', '"', '}',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_object_with_comment), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_object_with_trailing_garbage", a) {
    static const uint8_t n_structure_object_with_trailing_garbage[] = {
        '{', '"', 'a', '"', ':', ' ', 't', 'r', 'u', 'e', '}', ' ', '"', 'x', '"',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_object_with_trailing_garbage), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_open_array_apostrophe", a) {
    static const uint8_t n_structure_open_array_apostrophe[] = {
        '[',  39,
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_open_array_apostrophe), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_open_array_comma", a) {
    static const uint8_t n_structure_open_array_comma[] = {
        '[', ',',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_open_array_comma), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_open_array_open_object", a) {
    static const uint8_t n_structure_open_array_open_object[] = {
        '[', '{',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_open_array_open_object), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_open_array_open_string", a) {
    static const uint8_t n_structure_open_array_open_string[] = {
        '[', '"', 'a',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_open_array_open_string), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_open_array_string", a) {
    static const uint8_t n_structure_open_array_string[] = {
        '[', '"', 'a', '"',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_open_array_string), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_open_object", a) {
    static const uint8_t n_structure_open_object[] = {
        '{',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_open_object), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_open_object_close_array", a) {
    static const uint8_t n_structure_open_object_close_array[] = {
        '{', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_open_object_close_array), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_open_object_comma", a) {
    static const uint8_t n_structure_open_object_comma[] = {
        '{', ',',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_open_object_comma), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_open_object_open_array", a) {
    static const uint8_t n_structure_open_object_open_array[] = {
        '{', '[',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_open_object_open_array), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_open_object_open_string", a) {
    static const uint8_t n_structure_open_object_open_string[] = {
        '{', '"', 'a',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_open_object_open_string), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_open_object_string_with_apostrophes", a) {
    static const uint8_t n_structure_open_object_string_with_apostrophes[] = {
        '{',  39, 'a',  39,
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_open_object_string_with_apostrophes), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_open_open", a) {
    static const uint8_t n_structure_open_open[] = {
        '[', '"',  92, '{', '[', '"',  92, '{', '[', '"',  92, '{', '[', '"',  92, '{',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_open_open), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_single_point", a) {
    static const uint8_t n_structure_single_point[] = {
        233,
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_single_point), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_single_star", a) {
    static const uint8_t n_structure_single_star[] = {
        '*',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_single_star), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_trailing__", a) {
    static const uint8_t n_structure_trailing__[] = {
        '{', '"', 'a', '"', ':', '"', 'b', '"', '}', '#', '{', '}',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_trailing__), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_uescaped_LF_before_string", a) {
    static const uint8_t n_structure_uescaped_LF_before_string[] = {
        '[',  92, 'u', '0', '0', '0', 'A', '"', '"', ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_uescaped_LF_before_string), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_unclosed_array", a) {
    static const uint8_t n_structure_unclosed_array[] = {
        '[', '1',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_unclosed_array), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_unclosed_array_partial_null", a) {
    static const uint8_t n_structure_unclosed_array_partial_null[] = {
        '[', ' ', 'f', 'a', 'l', 's', 'e', ',', ' ', 'n', 'u', 'l',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_unclosed_array_partial_null), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_unclosed_array_unfinished_false", a) {
    static const uint8_t n_structure_unclosed_array_unfinished_false[] = {
        '[', ' ', 't', 'r', 'u', 'e', ',', ' ', 'f', 'a', 'l', 's',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_unclosed_array_unfinished_false), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_unclosed_array_unfinished_true", a) {
    static const uint8_t n_structure_unclosed_array_unfinished_true[] = {
        '[', ' ', 'f', 'a', 'l', 's', 'e', ',', ' ', 't', 'r', 'u',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_unclosed_array_unfinished_true), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_unclosed_object", a) {
    static const uint8_t n_structure_unclosed_object[] = {
        '{', '"', 'a', 's', 'd', '"', ':', '"', 'a', 's', 'd', '"',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_unclosed_object), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_unicode_identifier", a) {
    static const uint8_t n_structure_unicode_identifier[] = {
        195, 165,
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_unicode_identifier), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_whitespace_U_2060_word_joiner", a) {
    static const uint8_t n_structure_whitespace_U_2060_word_joiner[] = {
        '[', 226, 129, 160, ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_whitespace_U_2060_word_joiner), afl::except::FileFormatException);
}

AFL_TEST("afl.io.json.Parser:bad:n_structure_whitespace_formfeed", a) {
    static const uint8_t n_structure_whitespace_formfeed[] = {
        '[',  12, ']',
    };
    AFL_CHECK_THROWS(a, parseString(n_structure_whitespace_formfeed), afl::except::FileFormatException);
}


/*
 *  Test "ignore" cases. We have filtered them to cases we parse.
 */
AFL_TEST("afl.io.json.Parser:ignore:i_object_key_lone_2nd_surrogate", a) {
    static const uint8_t i_object_key_lone_2nd_surrogate[] = {
        '{', '"',  92, 'u', 'D', 'F', 'A', 'A', '"', ':', '0', '}',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(i_object_key_lone_2nd_surrogate));
}

AFL_TEST("afl.io.json.Parser:ignore:i_string_1st_surrogate_but_2nd_missing", a) {
    static const uint8_t i_string_1st_surrogate_but_2nd_missing[] = {
        '[', '"',  92, 'u', 'D', 'A', 'D', 'A', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(i_string_1st_surrogate_but_2nd_missing));
}

AFL_TEST("afl.io.json.Parser:ignore:i_string_1st_valid_surrogate_2nd_invalid", a) {
    static const uint8_t i_string_1st_valid_surrogate_2nd_invalid[] = {
        '[', '"',  92, 'u', 'D', '8', '8', '8',  92, 'u', '1', '2', '3', '4', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(i_string_1st_valid_surrogate_2nd_invalid));
}

AFL_TEST("afl.io.json.Parser:ignore:i_string_UTF_16_invalid_lonely_surrogate", a) {
    static const uint8_t i_string_UTF_16_invalid_lonely_surrogate[] = {
        '[', '"',  92, 'u', 'd', '8', '0', '0', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(i_string_UTF_16_invalid_lonely_surrogate));
}

AFL_TEST("afl.io.json.Parser:ignore:i_string_UTF_16_invalid_surrogate", a) {
    static const uint8_t i_string_UTF_16_invalid_surrogate[] = {
        '[', '"',  92, 'u', 'd', '8', '0', '0', 'a', 'b', 'c', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(i_string_UTF_16_invalid_surrogate));
}

AFL_TEST("afl.io.json.Parser:ignore:i_string_UTF_8_invalid_sequence", a) {
    static const uint8_t i_string_UTF_8_invalid_sequence[] = {
        '[', '"', 230, 151, 165, 209, 136, 250, '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(i_string_UTF_8_invalid_sequence));
}

AFL_TEST("afl.io.json.Parser:ignore:i_string_incomplete_surrogate_and_escape_valid", a) {
    static const uint8_t i_string_incomplete_surrogate_and_escape_valid[] = {
        '[', '"',  92, 'u', 'D', '8', '0', '0',  92, 'n', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(i_string_incomplete_surrogate_and_escape_valid));
}

AFL_TEST("afl.io.json.Parser:ignore:i_string_incomplete_surrogate_pair", a) {
    static const uint8_t i_string_incomplete_surrogate_pair[] = {
        '[', '"',  92, 'u', 'D', 'd', '1', 'e', 'a', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(i_string_incomplete_surrogate_pair));
}

AFL_TEST("afl.io.json.Parser:ignore:i_string_incomplete_surrogates_escape_valid", a) {
    static const uint8_t i_string_incomplete_surrogates_escape_valid[] = {
        '[', '"',  92, 'u', 'D', '8', '0', '0',  92, 'u', 'D', '8', '0', '0',  92, 'n', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(i_string_incomplete_surrogates_escape_valid));
}

AFL_TEST("afl.io.json.Parser:ignore:i_string_inverted_surrogates_U_1D11E", a) {
    static const uint8_t i_string_inverted_surrogates_U_1D11E[] = {
        '[', '"',  92, 'u', 'D', 'd', '1', 'e',  92, 'u', 'D', '8', '3', '4', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(i_string_inverted_surrogates_U_1D11E));
}

AFL_TEST("afl.io.json.Parser:ignore:i_string_lone_second_surrogate", a) {
    static const uint8_t i_string_lone_second_surrogate[] = {
        '[', '"',  92, 'u', 'D', 'F', 'A', 'A', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(i_string_lone_second_surrogate));
}

AFL_TEST("afl.io.json.Parser:ignore:i_string_not_in_unicode_range", a) {
    static const uint8_t i_string_not_in_unicode_range[] = {
        '[', '"', 244, 191, 191, 191, '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(i_string_not_in_unicode_range));
}

AFL_TEST("afl.io.json.Parser:ignore:i_string_truncated_utf_8", a) {
    static const uint8_t i_string_truncated_utf_8[] = {
        '[', '"', 224, 255, '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(i_string_truncated_utf_8));
}

AFL_TEST("afl.io.json.Parser:ignore:i_string_unicode_U_10FFFE_nonchar", a) {
    static const uint8_t i_string_unicode_U_10FFFE_nonchar[] = {
        '[', '"',  92, 'u', 'D', 'B', 'F', 'F',  92, 'u', 'D', 'F', 'F', 'E', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(i_string_unicode_U_10FFFE_nonchar));
}

AFL_TEST("afl.io.json.Parser:ignore:i_string_unicode_U_1FFFE_nonchar", a) {
    static const uint8_t i_string_unicode_U_1FFFE_nonchar[] = {
        '[', '"',  92, 'u', 'D', '8', '3', 'F',  92, 'u', 'D', 'F', 'F', 'E', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(i_string_unicode_U_1FFFE_nonchar));
}

AFL_TEST("afl.io.json.Parser:ignore:i_string_unicode_U_FDD0_nonchar", a) {
    static const uint8_t i_string_unicode_U_FDD0_nonchar[] = {
        '[', '"',  92, 'u', 'F', 'D', 'D', '0', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(i_string_unicode_U_FDD0_nonchar));
}

AFL_TEST("afl.io.json.Parser:ignore:i_string_unicode_U_FFFE_nonchar", a) {
    static const uint8_t i_string_unicode_U_FFFE_nonchar[] = {
        '[', '"',  92, 'u', 'F', 'F', 'F', 'E', '"', ']',
    };
    AFL_CHECK_SUCCEEDS(a, parseString(i_string_unicode_U_FFFE_nonchar));
}

AFL_TEST("afl.io.json.Parser:ignore:i_structure_500_nested_arrays", a) {
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
    AFL_CHECK_SUCCEEDS(a, parseString(i_structure_500_nested_arrays));
}

