#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC_BUILDS'}/lib";

my $nl;

while (<>) {
    s/^\s*$//;
    if (/^$/) {
	print "\n" unless $nl++;
    } else {
	print;
	$nl = 0;
    }
}

1;
