#!/usr/bin/perl
use warnings; use strict;
my %tr = ();
while (<>) {
    m#/([^/]+)_.*?-(.*?)\.# && ++${$tr{$1}}{$2};
}

foreach my $pqx (sort keys %tr) {
    print "$pqx\t", join(' ', sort keys %{$tr{$pqx}}), "\n";
}

1;
