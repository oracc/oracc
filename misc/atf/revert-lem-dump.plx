#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

while (<>) {
    chomp;
    my($l,$f) = split(/\t/, $_);
    foreach my $w (split(/\s+/, $f)) {
	print "$w\t$l\n";
    }
}

1;
