#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::Words;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;

my %args = pp_args();

words_check(\%args);

if (pp_status()) {
    pp_diagnostics();
    exit 1;
} else {
    warn "cbdwords.plx: words in $args{'cbd'} have no detectable duplicates or overlaps.\n";
    exit 0;
}

1;
