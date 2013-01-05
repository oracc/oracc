#!/usr/bin/perl
use warnings; use strict;

my %met = ();
my @met = qw/gur sila3/;
@met{@met} = ();

while (<>) {
    s/^\S+\s*//;
    next if m/^(?:iti|u4)\s+/;
    next if m/^mu\s+/ && !m/-sze3/;
    chomp;
    tr/[].#*!?<>//d;
    my @words = split(/\s+/,$_);
    my $i;
    for ($i = 0; $i <= $#words; ++$i) {
	last unless num_or_met($words[$i]);
    }
    print "@words[$i..$#words]\n" unless $i == $#words;
}

sub
num_or_met {
    $_[0] =~ /^(:?n|\d+(?:\/\d)?)\(/ || exists $met{$_[0]};
}

1;
