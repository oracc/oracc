#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::SL::Base;

binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
ORACC::SL::Base::init();
while (<>) {
    chomp;
    print "$_\t",ORACC::SL::Base::tlit2cunei('<stdin>',ORACC::SL::Base::tlitsplit($_,1)), "\n";
}
ORACC::SL::Base::term();
1;
