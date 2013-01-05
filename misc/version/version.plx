#!/usr/bin/perl
use warnings; use strict;

my $nextV = 'v000';
my @V = grep(-d $_, <v[0-9][0-9][0-9]>);
if ($#V >= 0) {
    my $lastV = (sort @V)[$#V];
    $lastV =~ /^v0*(\d+)/;
    $nextV = sprintf("v%03d", 1 + $1);
}

print $nextV;

1;
