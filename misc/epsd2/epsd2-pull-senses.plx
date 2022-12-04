#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

my $cf;
while (<>) {
    if (/^\@entry[!\*]*\s+(.*)\s*$/) {
	$cf = $1;
	$cf =~ s/\s*$//;
    } elsif (/^\@sense/) {
	print "$cf\t$_";
    }
}

1;
