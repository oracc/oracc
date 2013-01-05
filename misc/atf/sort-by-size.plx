#!/usr/bin/perl
use warnings; use strict;

my $currtext = undef;
my $nlines;
my $ngraph;
my $nwords;

while (<>) {
    if (/^\&(\S+)/) {
	dumpinfo();
	$currtext = $1;
    } elsif (/^\d/) {
	++$nlines;
	$nwords += tr/ / /;
	$ngraph += tr/-. \{/-. \{/;
    }
}
dumpinfo();

sub
dumpinfo {
    print "$currtext\t$nlines\t$nwords\t$ngraph\n" if $currtext;
    $nlines = $nwords = $ngraph = 0;
}

1;
