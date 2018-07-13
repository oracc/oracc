#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::XML;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::CBD::Equivs;

my %args = pp_args();
$ORACC::CBD::nonormify = 1;

my @base_cbd = ();
if ($args{'base'}) {
    @base_cbd = setup_cbd(\%args,$args{'base'});
} else {
    die "$0: must give base glossary with -base GLOSSARY\n";
}

my @cbd = setup_cbd(\%args);

@cbd = equivs_resolve(\%args, \@base_cbd, \@cbd);

if (pp_status()) {
    pp_diagnostics();
} else {
    pp_cbd(\%args,@cbd) unless $args{'check'};
}

1;
