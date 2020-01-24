#!/usr/bin/perl
use warnings; use strict;
my $file = shift @ARGV;
my @keep = ();
my %seen = ();

open(X,$file);
while (<X>) {
    push @keep, $_ unless $seen{$_}++
}
close(X);
open(X,">$file");
print X @keep;
close(X);

1;
