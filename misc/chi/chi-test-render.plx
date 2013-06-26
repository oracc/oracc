#!/usr/bin/perl
use warnings; use strict;
use Data::Dumper;
use lib '@@ORACC@@/lib';
use ORACC::CHI::Render;

while (<>) {
    chomp;
    my $res = ORACC::CHI::Render::un_chi($_);
    print "$_ -> $res\n";
}

1;
