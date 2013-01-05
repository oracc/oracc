#!/usr/bin/perl
use warnings; use strict;
my $print = 0;
while (<>) {
    if (/d/) {
	$print = 1;
    } elsif (/a/) {
	$print = 0;
    } else {
	print if $print;
    }
}

1;
