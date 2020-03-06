#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::SL::BaseC;

binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
ORACC::SL::BaseC::init();
while (<>) {
    chomp;
    print "$_\t",ORACC::SL::BaseC::tlit2cunei('<stdin>',ORACC::SL::BaseC::tlitsplit($_,1)), "\n";
}
ORACC::SL::BaseC::term();
1;
