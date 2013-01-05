#!/usr/bin/perl
use warnings; use strict; use open ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::Legacy::Sexify;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

foreach my $n (@ARGV) {
    print "$n -> ", ORACC::Legacy::Sexify::sexify($n,0,1,1), "\n";
}

1;
