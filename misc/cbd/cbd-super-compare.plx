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

## No: we are working on 01tmp/xxx.glo where the normification
## has already been done, so we want to do it correspondingly to
## incoming glossaries
## $ORACC::CBD::nonormify = 1;

my %args = pp_args();

my @base_cbd = ();

if ($args{'base'}) {
    if ($args{'base'} =~ /00src/) {
	die "$0: -base should be a cbd with internal forms\n";
    } else {
	@base_cbd = setup_cbd(\%args,$args{'base'});
	if (pp_status()) {
	    pp_diagnostics();
	    die "$0: can't align bases unless base glossary is clean. Stop.\n";
	}
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
if ($args{'dynamic'}) {
    system 'mkdir', '-p', '01map';
    $mapfile = '01map/'.$mapfile;
} else {
    $mapfile = '00map/'.$mapfile;
}
$mapfile .= '~'.lang($args{'cbd'}).'.map';

if ($args{'stdout'}) {
    *MAP_FH = *STDOUT;
} else {
    open(MAP_FH,">$mapfile") || die "$0: unable to open $mapfile for output. Stop.\n";
}
entries_align(\%args, \@base_cbd, \@cbd, \*MAP_FH);
senses_align(\%args, \@base_cbd, \@cbd, \*MAP_FH);
bases_align(\%args, \@base_cbd, \@cbd, \*MAP_FH);
forms_align(\%args, \@base_cbd, \@cbd, \*MAP_FH);
close(MAP_FH);

if ($args{'apply'}) {
    my @exec_args = ();
    push @exec_args, '-inplace' if $args{'inplace'};
    push @exec_args, '-increment', $args{'increment'} if defined $args{'increment'};
    push @exec_args, '-base', $args{'base'}, $mapfile;
    warn "cbd-super-compare.plx: applying @exec_args\n";
    exec "$ENV{'ORACC_BUILDS'}/bin/cbd-super-map-glos.plx", @exec_args;
}

1;
