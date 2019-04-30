#!/usr/bin/perl -w
#
#  Optimize coverage
#
#  Usage:
#    coverage_optimize.pl < file.info > file_new.info
#
#  Because gcc creates multiple instances of destructors, it is hard
#  to achieve 100% function coverage as measured by gcov, even if you
#  actually have 100% source code coverage. This script optimizes the
#  coverage report by removing unreferenced destructors if another
#  instance is covered.
#

my %record;
my %seq;
my $seqNo;
while (<>) {
    if (/^([A-Z]*):(.*)/) {
        if (!$record{$1}) {
            $seq{$1} = $seqNo++;
        }
        push @{$record{$1}}, $2;
    } elsif (/^end_of_record/) {
        processRecord();
        %record = ();
        %seq = ();
        $seqNo = 0;
    } else {
        print STDERR "WARNING: unparsed line $.: $_";
    }
}
if (keys %record) {
    print STDERR "WARNING: partial record at end\n";
}


sub processRecord {
    my $fn = $record{FN};
    my $fnda = $record{FNDA};
    my $fnf = $record{FNF};
    my $fnh = $record{FNH};

    if ($fnf && $fnh && @$fnf == 1 && @$fnh == 1) {
        if ($fn && $fnda) {
            my $i = 0;
            while ($i < @$fnda) {
                if ($fnda->[$i] =~ /^0,(.*)(D\dEv)$/) {
                    # An unreferenced destructor.
                    # Drop it from both lists if there is another destructor of the same name.
                    my $baseName = $1;
                    my $end = $2;
                    my $nOther = (grep {/^\d+,\Q$baseName\ED\dEv$/} @$fnda);
                    my $nSelf = (grep {/^\d+,\Q$baseName$end\E$/} @$fn);
                    if ($nOther > 1 && $nSelf == 1) {
                        splice @$fnda, $i, 1;
                        @$fn = grep {!/^\d+,\Q$baseName$end\E$/} @$fn;
                        --$fnf->[0];
                        --$fnh->[0];
                    } else {
                        ++$i;
                    }
                } else {
                    ++$i;
                }
            }
        }
    } else {
        print STDERR "WARNING: unable to interpret record ending at line $.\n";
    }

    foreach my $k (sort {$seq{$a} <=> $seq{$b}} keys %record) {
        print map{"$k:$_\n"} @{$record{$k}};
    }
    print "end_of_record\n";
}
