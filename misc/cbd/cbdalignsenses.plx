#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::XML;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::CBD::Senses;

my %args = pp_args();

my @base_cbd = ();
if ($args{'base'}) {
    @base_cbd = setup_cbd(\%args,$args{'base'});
} else {
    die "$0: must give base glossary with -base GLOSSARY\n";
}

my @cbd = setup_cbd(\%args);

senses_init(\%args);
senses_align(\%args, \@base_cbd, \@cbd);
senses_term();

### need to output revised glo here if -apply is given

pp_diagnostics() if pp_status();

1;
