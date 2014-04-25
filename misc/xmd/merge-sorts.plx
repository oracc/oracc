#!/usr/bin/perl
use warnings; use strict;

my %q = ();

while (<>) {
    chomp;
    my($q,$s) = split(/\t/,$_);
    $q{$q} = $s;
}

foreach my $q (sort keys %q) {
    print "$q\t$q{$q}\n";
}

1;
