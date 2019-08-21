#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::XML;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::CBD::Entries;
my $acd_rx = $ORACC::CBD::acd_rx;

my %args = pp_args();

$ORACC::CBD::nonormify = 1;

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

entries_init(\%args);
my %map = entries_align(\%args, \@base_cbd, \@cbd);
entries_term();

if ($args{'apply'}) {
    my $mapto = '';
    for (my $i = 0; $i <= $#cbd; ++$i) {
	if ($cbd[$i] =~ /^$acd_rx\@entry\s+(.*?)\s*$/) {
	    if ($map{$1}) {
		$mapto = $map{$1};
	    }
	}
	print $cbd[$i], "\n" unless $cbd[$i] =~ /^\000/;
	if ($mapto) {
	    print ">$mapto\n";
	    $mapto = '';
	}
    }
} else {
    pp_diagnostics();
}

1;
