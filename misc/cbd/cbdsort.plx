#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::Sort;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;

my %args = pp_args();
my @cbd = setup_cbd(\%args);

if (pp_status()) {
    pp_diagnostics();
} else {
    pp_cbd(\%args, sort_cbd(\%args, @cbd));
}

1;
