#!/usr/bin/perl
use warnings; use strict;

my $ands = '';
my %ands = ();

# Replace &-lines with the versions given in the -ands arg; if the
# P/Q/X number doesn't occur in -ands file use the existing &-line.

use Getopt::Long;
GetOptions(
    'ands:s'=>\$ands,
    );

if ($ands) {
    open(K, $ands) || die;
    while (<K>) {
	chomp;
	s/^&//;
	my $P = $_;
	$P =~ s/\s+.*$//;
	$ands{$P} = $_;
    }
    close(K);
}

my %texts = ();
$/ = "\n\&";

my @file = (<>);
my @order = ();

foreach (@file) {
    s/^[\n\t ]*\&?/&/;
    s/\&$//;
    s/^\s+//x;
    s/\s+$//x;
    /\&([PQX]\d+)/;
    my $P = $1;
    push @order, $P;
    $texts{$P} = $_;
}
 
foreach my $P (@order) {
    if ($ands{$P}) {
	my $a = $texts{$P};
	$a =~ s/\&.*?\n//;
	print '&', $ands{$P}, "\n", $a;
    } else {
        print $texts{$P};
    }
    print "\n\n\n";
}

1;
