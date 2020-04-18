#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

binmode $DB::OUT, ':utf8' if $DB::OUT;

use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::SL::BaseC;
ORACC::SL::BaseC::init();

my $t = shift @ARGV; Encode::_utf8_on($t);

my $s = ORACC::SL::BaseC::tlit_sig('',$t);

print "$t => $s\n";

1;
