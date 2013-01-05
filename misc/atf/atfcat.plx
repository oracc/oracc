#!/usr/bin/perl
use warnings; use strict;

foreach my $f (@ARGV) {
    if ($f =~ /\.ods$/) {
	system 'ods2atf.sh', '-s', $f;
    } else {
	system 'cat', $f;
    }
    print "\n";
}

1;
