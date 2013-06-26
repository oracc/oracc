#!/usr/bin/perl
use warnings; use strict;
use Data::Dumper;
use lib '@@ORACC@@/lib';
use ORACC::CHI::Maker;
use ORACC::CHI::Sorter;
use ORACC::CHI::Splitter;

while (<>) {
    chomp;
    my @split = ORACC::CHI::Splitter::split_set($_);
    my @sorted = ORACC::CHI::Sorter::sortuniq_split_set(@split);
    my $sorted = ORACC::CHI::Splitter::join_set(@sorted);
    print "$_ -> $sorted\n";
}

1;
