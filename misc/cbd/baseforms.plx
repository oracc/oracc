#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

my $lang = '%sux-x-emesal ';

while (<>) {
    if (s/^\@bases\s+//) {
	chomp;
	s/\s+\(.*?\);/;/g;
	s/\s+\(.*?\)\s*$//;
	my @b = split(/;\s+/, $_);
	foreach my $b (@b) {
	    print "\@form $b $lang /$b #~\n";
	}
    }
}

1;
