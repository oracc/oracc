#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

my %i = ();

while (<>) {
    chomp;
    my($f,$l) = split(/\t/, $_);
    ++${$i{$l}}{$f};
}

foreach my $i (sort keys %i) {
    print "$i\t", join(' ', sort keys %{$i{$i}}), "\n";
}

1;
