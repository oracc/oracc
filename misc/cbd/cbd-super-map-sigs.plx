#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::CBD::Map;

use Data::Dumper;

$ORACC::CBD::nosetupargs = 1;
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

my %map = map_load(shift @ARGV,'sigs');

#print Dumper \%map; exit 1;

while (<>) {
    if (/^\@[^%]+\s/ || /^\s*$/ || /^#/) {
	print;
    } else {
	print map_apply_sig($_);
    }
}


1;
