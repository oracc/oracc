#!/usr/bin/perl
use warnings; use strict; use open 'utf8';

my $t = shift @ARGV;
my $x = $t; $x =~ s/.[^.]+$/.xml/;
open(X,">$x");
print X '<tab>';
open(T,$t);
while (<T>) {
    chomp;
    my @f = split(/\t/,$_);
    print X "<t k=\"$f[0]\" v=\"$f[1]\"/>";
}
close(T);
print X '</tab>';
close(X);
1;
