#!/usr/bin/perl -w
#
#  This script was used to generate the tables in unicode.cpp.
#
use strict;

my %upper;
my %lower;

while (<>) {
    # Read a line
    my ($code, $name, $category, $combiningClass, $bidiClass, $decomp, $num1, $num2, $num3, $bidiMirrored, $oldName, $comment, $upper, $lower, $title) = split /;/;
    next if $code =~ /[^0-9A-F]/;

    # Decode
    my $codeNr = hex($code);
    my $upperNr = hex($upper);
    my $lowerNr = hex($lower);

    # Process only a limited range for now.
    if (isValid($codeNr)) {
        $upper{$codeNr} = $upperNr
            if isValid($upperNr);
        $lower{$codeNr} = $lowerNr
            if isValid($lowerNr);
    }
}

# printf "// %d upper, %d lower\n",
#     scalar(keys %upper),
#     scalar(keys %lower);
#
# foreach (sort {$a<=>$b} keys %lower) {
#     if ($upper{$lower{$_}} != $_) {
#         printf "// U+%04X's lower-case letter does not map back\n", $_;
#     }
# }
#
# foreach (sort {$a<=>$b} keys %upper) {
#     if ($lower{$upper{$_}} != $_) {
#         printf "// U+%04X's upper-case letter does not map back\n", $_;
#     }
# }

buildMapping('upper', \%upper);
buildMapping('lower', \%lower);


sub buildMapping {
    my ($name, $p) = @_;
    my $char = 0;               # Input character
    my $delta = 0;              # Difference between input and output character
    my $offset = 0;             # Offset between characters in sequence
    my $count = 0;              # Number of characters in sequence
    printf "const Mapping %s_table[] = {\n", $name;
    foreach (sort {$a<=>$b} keys %$p) {
        my $thisDelta = $p->{$_} - $_;
        if ($thisDelta == 0) {
            # No need to store
        } elsif ($count == 1 && $thisDelta == $delta && isPowerOf2($_ - $char)) {
            # Continue a sequence of length 1
            $offset = $_ - $char;
            ++$count;
        } elsif ($count > 1 && $thisDelta == $delta && $_ == $char + $offset*$count) {
            # Continue a sequence of length >1
            ++$count;
        } else {
            # Start a new sequence
            if ($count > 0) {
                printf "    { 0x%04X, 0x%04X, %4d, %4d },\n", $char, $char+$delta, $offset, $count;
            }
            $char = $_;
            $delta = $thisDelta;
            $offset = 1;
            $count = 1;
        }
    }
    if ($count > 0) {
        printf "    { 0x%04X, 0x%04X, %4D, %4d },\n", $char, $char+$delta, $offset, $count;
    }
    printf "};\n\n";
}

# Check for valid character.
# We process only a limited range.
# This saves some memory by not having to allocate full 32 bits for offset and character for things nobody uses.
#   U+10400        DESERET CAPITAL LETTER...  - only case pair above U+10000
#   U+0265/U+A78D  LATIN...LETTER TURNED H    - a pair with >32k difference between members
sub isValid {
    my $n = shift;
    return $n > 0 && $n < 0x3000;
}

sub isPowerOf2 {
    my $n = shift;
    return ($n & ($n-1)) == 0;
}
