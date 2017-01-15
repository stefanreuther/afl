#!/usr/bin/perl -w
#
#  JSON Test-Suite Generator
#
#  This turns the JSON testsuite from https://github.com/nst/JSONTestSuite into a unit test.
#  Usage:
#  + check out the above repo
#  + run this script as
#        perl generate_json_testsuite.pl /path/to/JSONTestSuite > u/t_io_json_parser_testsuite.cpp
#
use strict;
use bytes;

#
#  Configuration
#

my $dir = shift @ARGV or die "please give me the input directory";

#
#  Blacklists (tests we know we fail)
#
my @BLACKLIST = (
    'y_number_double_huge_neg_exp',           # exponent overflow
    'y_number_huge_exp',                      # exponent overflow
    'y_number_real_neg_overflow',             # exponent overflow
    'y_number_real_pos_overflow',             # exponent overflow
    'y_number_real_underflow',                # exponent overflow
    'y_string_utf16',                         # we only do UTF-8
    'n_structure_no_data',                    # already tested in manual test, avoids having to add special case to this code
    'n_structure_100000_opening_arrays',      # needs 14M stack (Linux has 8M), which we don't defend against yet. Segfaults. FIXME!
    'n_structure_open_array_object',          # similar

    # These fail because we leave number parsing to libc:
    'n_number__01',                           # -01
    'n_number__2_',                           # -2.
    'n_number_0_e1',                          # 0.e1
    'n_number_2_e_3',                         # 2.e+3
    'n_number_2_e_3_1',                       # 2.e-3
    'n_number_2_e3',                          # 2.e3
    'n_number_neg_int_starting_with_zero',    # -012
    'n_number_neg_real_without_int_part',     # -.123
    'n_number_real_without_fractional_part',  # 1.
    'n_number_with_leading_zero',             # 012

    # These fail because we don't validate UTF-8:
    'n_string_UTF8_surrogate_U_D800',
    'n_string_invalid_utf_8',
    'n_string_iso_latin_1',
    'n_string_lone_utf8_continuation_byte',
    'n_string_overlong_sequence_2_bytes',
    'n_string_overlong_sequence_6_bytes',
    'n_string_overlong_sequence_6_bytes_null',

    # "Ignore" cases which we fail:
    'i_number_neg_int_huge_exp',              # huge exponent (already appears in similar form in 'y' series)
    'i_number_pos_double_huge_exp',           # same
    'i_structure_UTF_8_BOM_empty_object',     # we don't interpret characters, so we don't know what a BOM is
    );
my %BLACKLIST;
foreach (@BLACKLIST) {
    $BLACKLIST{$_} = 1
}

#
#  Generator
#
my %hadThisName;

print <<'EOF';
/**
  *  \file u/t_io_json_parser_testsuite.cpp
  *  \brief Test for afl::io::json::Parser - Testsuite
  *
  *  GENERATED FILE, do not edit. See scripts/generate_json_testsuite.pl.
  *
  *  Uses the test suite from https://github.com/nst/JSONTestSuite which comes with this license:
  *
EOF

open LICENSE, "< $dir/LICENSE" or die "license";
while (<LICENSE>) {
    s/[\r\n]//g;
    if ($_ eq '') {
        print "  *\n";
    } else {
        print "  *  $_\n";
    }
}
close LICENSE;

print <<'EOF';
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
EOF

foreach (sort <$dir/test_parsing/y_*.json>) {
    my $name = testName($_);
    next if $BLACKLIST{$name};
    dumpFile($_, $name);
    print "    TS_ASSERT_THROWS_NOTHING(parseString($name));\n\n";
    close FILE;
}

print <<'EOF';
}

/** Test "bad" cases. These must all fail. */
void
TestIoJsonParser::testTestsuiteBad()
{
EOF

foreach (sort <$dir/test_parsing/n_*.json>) {
    my $name = testName($_);
    next if $BLACKLIST{$name};
    dumpFile($_, $name);
    print "    TS_ASSERT_THROWS(parseString($name), afl::except::FileFormatException);\n\n";
    close FILE;
}

print <<'EOF';
}

/** Test "ignore" cases. We have filtered them to cases we parse. */
void
TestIoJsonParser::testTestsuiteUgly()
{
EOF

foreach (sort <$dir/test_parsing/i_*.json>) {
    my $name = testName($_);
    next if $BLACKLIST{$name};
    dumpFile($_, $name);
    print "    TS_ASSERT_THROWS_NOTHING(parseString($name));\n\n";
    close FILE;
}

print <<'EOF';
}
EOF











sub testName {
    my $x = shift;
    $x =~ s|.*/||;
    $x =~ s|\.json$||;
    $x =~ s|\W+|_|g;
    my $n = $hadThisName{$x}++;
    if ($n) {
        $x .= "_$n";
    }
    $x;
}

sub dumpFile {
    my $file = shift;
    my $name = shift;
    print "    static const uint8_t $name", "[] = {\n";
    open FILE, "< $file" or die "$file: $!";
    binmode FILE;
    my $x;
    while (read(FILE, $x, 20)) {
        print "       ";
        foreach (unpack "C*", $x) {
            if ($_ >= 32 && $_ < 127 && $_ != 39 && $_ != 92) {
                printf " '%c',", $_;
            } else {
                printf " %3d,", $_;
            }
        }
        print "\n";
    }
    print "    };\n";
}
