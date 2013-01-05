#!/usr/bin/perl
use Number::Fraction;

my $n = Number::Fraction->new('3/2');

my $p = eval($n);

print "3/2 = $p\n";

my $m = Number::Fraction->new('5/6');
my $l = Number::Fraction->new('1/6');

print "m*l = ", $m*$l, "\n";
print "m/l = ", $m/$l, "\n";

my $x = Number::Fraction->new('19602/100000');
print "19602/100000 = $x\n";
