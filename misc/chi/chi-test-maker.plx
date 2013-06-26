#!/usr/bin/perl
use warnings; use strict;
use Data::Dumper;
use lib '@@ORACC@@/lib';
use ORACC::CHI::Maker;

while (<>) {
    chomp;
    my $res = ORACC::CHI::Maker::set($.,$_);
    print "$_ -> $res\n";
}

1;
