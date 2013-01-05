#!/usr/bin/perl
use warnings; use strict;

my %texts = ();
$/ = "\n\&";

my @file = (<>);

foreach (@file) {
    s/^\&?/&/;
    s/\&$//;
    /^.*=\s*(.*)\s*$/m;
    $texts{$1} = $_;
}

foreach my $t (sort { &tcmp } keys %texts) {
    print $texts{$t};
}

sub
tcmp {
    $a cmp $b;
}

1;
