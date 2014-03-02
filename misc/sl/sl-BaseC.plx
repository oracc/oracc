#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::SL::BaseC;

ORACC::SL::BaseC::init();
while (<>) {
    chomp;
    my $res = ORACC::SL::BaseC::sign_of($_);
    print "$res\n";
}
ORACC::SL::BaseC::term();

1;
