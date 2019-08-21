#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::Bases;
use ORACC::CBD::Util;


my %args = pp_args();
$ORACC::CBD::noforms = 1;
my @cbd = setup_cbd(\%args);

ORACC::CBD::Bases::bases_tab(@cbd);

1;
