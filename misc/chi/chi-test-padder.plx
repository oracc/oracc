#!/usr/bin/perl
use warnings; use strict;

use lib '@@ORACC@@/lib';
use ORACC::CHI::Padder;

while (<>) {
    chomp;
    my $pad = ORACC::CHI::Padder::pad($.,$_);
    print "$_ => $pad\n";
}

1;
