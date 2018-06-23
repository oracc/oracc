#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::XML;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::CBD::Bases;

# $ORACC::CBD::nosetupargs = 1;
my %args = pp_args();

my @base_cbd = ();

if ($args{'base'}) {
    @base_cbd = setup_cbd(\%args,$args{'base'});
    if (pp_status()) {
	pp_diagnostics();
	die "$0: can't align bases unless base glossary is clean. Stop.\n";
    }
} else {
    die "$0: must give base glossary with -base GLOSSARY\n";
}

my @cbd = setup_cbd(\%args);

if (pp_status()) {
    pp_diagnostics();
    die "$0: can't align bases unless incoming glossary is clean. Stop.\n";
}

bases_init(\%args);
bases_align(\%args, \@base_cbd, \@cbd);
bases_term();

1;
