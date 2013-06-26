#!/usr/bin/perl
use warnings; use strict;
use Data::Dumper;
use lib '@@ORACC@@/lib';
use ORACC::CHI::Maker;
use ORACC::CHI::Merger;
use ORACC::CHI::Sorter;
use ORACC::CHI::Splitter;

while (<>) {
    chomp;
    next if /^\s*$/;
    my ($set, $add, $where) = split(/\s+/, $_);
    my $res = ORACC::CHI::Merger::merge($add, $set, $where);
    print "$_ -> $res\n";
}

1;
