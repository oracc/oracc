#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::XML;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::CBD::Forms;

my %args = pp_args();
$ORACC::CBD::nominusstripping = $ORACC::CBD::nondestructive = $ORACC::CBD::nonormify = 1;
my @base_cbd = ();

set_default_base(\%args) unless $args{'base'};

if ($args{'base'}) {
    @base_cbd = setup_cbd(\%args,$args{'base'});
    warn "FILE:CORE=$args{'base'}\n";
    if (pp_status()) {
	pp_diagnostics();
	die "$0: can't align forms unless base glossary is clean. Stop.\n";
    }
} elsif ($args{'forms'}) {
    warn "FILE:CORE=$args{'forms'}\n";
    # forms_compare will figure this out
} else {
    die "$0: must give base glossary with -base GLOSSARY\n";
}

my @cbd = setup_cbd(\%args);
warn "FILE:PERI=".pp_file()."\n";

if (pp_status()) {
    pp_diagnostics();
    die "$0: can't align forms unless incoming glossary is clean. Stop.\n";
}

forms_init(\%args);
forms_compare(\%args, \@base_cbd, \@cbd);
forms_term();

pp_diagnostics() if pp_status();

1;
