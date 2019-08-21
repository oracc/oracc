#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use lib "$ENV{'ORACC'}/lib";
use ORACC::CBD::Util;
my $acd_rx = $ORACC::CBD::acd_rx;

my $cf;
while (<>) {
    if (/^$acd_rx\@entry\*?\s+(.*)\s*$/) {
	$cf = $1;
	$cf =~ s/\s*$//;
    } elsif (/^\@form/) {
	print "$cf\t$_";
    }
}

1;
