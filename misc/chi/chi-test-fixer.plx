#!/usr/bin/perl
use warnings; use strict;

use lib '@@ORACC@@/lib';
use ORACC::CHI::Fixer;

while (<>) {
    chomp;
    my @splitted = split(/\s*\(?[+&][\)?!*]*\s+/, $_);
    @splitted = ORACC::CHI::Fixer::fix_joins(@splitted);
    print join(' :: ', @splitted), "\n";
}

1;
