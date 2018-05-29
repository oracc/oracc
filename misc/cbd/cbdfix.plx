#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::Fix;
use ORACC::CBD::Hash;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;

$ORACC::CBD::nonormify = 1;

my %args = pp_args();

my $h = '';

if (($h = pp_hash(\%args))) {
    pp_fix(\%args, $h);
} else {
    pp_diagnostics();
}

1;
