#!/usr/bin/perl
use warnings; use strict;
use Data::Dumper;
use lib '@@ORACC@@/lib';
use ORACC::CHI::Splitter;

open(D, ">test-splitter.dump");
while (<>) {
    chomp;
    my @s = ORACC::CHI::Splitter::split_set($_);
    print D Dumper(@s);
    print ORACC::CHI::Splitter::join_set(@s), "\n";
}
close(D);

1;
