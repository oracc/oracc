#!/usr/bin/perl
use warnings; use strict;

my %nums = ();

open(L,shift @ARGV);
while (<L>) {
    if (/:(\d+):.*?criteria/) {
	++$nums{$1};
    }
}
close(L);

while (<>) {
    my $x = sprintf("%d",$.);
    if (defined $nums{$x}) {
	chomp;
	s/\$\s+\((.*?)\)\s*$/\$ $1\n/;
    }
    print;
}

1;
