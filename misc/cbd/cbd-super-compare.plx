#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::XML;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::CBD::Entries;
use ORACC::CBD::Senses;
use ORACC::CBD::Bases;
use ORACC::CBD::Forms;

$ORACC::CBD::nonormify = 1;

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

my $mapfile = project($args{'cbd'});
$mapfile =~ tr#/#-#;
$mapfile = '00map/'.$mapfile;
$mapfile .= '~'.lang($args{'cbd'}).'.map';

open(MAP_FH,">$mapfile") || die "$0: unable to open $mapfile for output. Stop.\n";    
entries_align(\%args, \@base_cbd, \@cbd, \*MAP_FH);
senses_align(\%args, \@base_cbd, \@cbd, \*MAP_FH);
bases_align(\%args, \@base_cbd, \@cbd, \*MAP_FH);
forms_align(\%args, \@base_cbd, \@cbd, \*MAP_FH);
close(MAP_FH);

1;
