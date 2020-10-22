#!/usr/bin/perl
use warnings; use strict; use open ':utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::SL::BaseC;

ORACC::SL::BaseC::init();

while (<>) {
    chomp;
    /\t(.*?)$/ || /^(.*)$/;
    my $t = $1;
    my $s = ORACC::SL::BaseC::tlit_sig('',$t);
    my $st = join(' ',@ORACC::SL::BaseC::last_tlit);
    print "$_\t$s\t$st\n";
}

1;
