#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use Data::Dumper;

use ORACC::CBD::XML;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Entries;
use ORACC::CBD::Util;
my $acd_rx = $ORACC::CBD::acd_rx;

my %args = pp_args();

$ORACC::CBD::nonormify = 1;

my @base_cbd = ();

set_default_base(\%args) unless $args{'base'};

if ($args{'base'}) {
    @base_cbd = setup_cbd(\%args,$args{'base'});
    if (pp_status()) {
	pp_diagnostics(\%args);
	die "$0: can't align bases unless base glossary is clean. Stop.\n";
    }
    $args{'lang'} = lang() unless $args{'lang'};
    $args{'output'} = "$args{'lang'}-entries-aligned.glo" unless $args{'output'};
} else {
    die "$0: must give base glossary with -base GLOSSARY\n";
}

$args{'bases'} = 1; # Force generation of 01bld/<LANG>/base-sigs.tab

my @cbd = setup_cbd(\%args);

if (pp_status()) {
    pp_diagnostics(\%args);
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
		if ($cbd[$i+1] =~ /^>/) {
		    my ($s) = ($cbd[$i+1] =~ /^>\s*(.*?)\s*$/);
		    if ($s ne $mapto) {
			pp_line($i);
			pp_warn("map-to entry in > line differs from stored sense ($s ne $mapto)");
			$mapto = undef;
		    } else {
			++$i;
		    }
		} else {
		    $mapto =~ s/\@entry\s+// if $mapto; # new style drops @entry/@sense after >
		    $cbd[$i] .= "\n>$mapto";
		}
	    }
	}
    }
    $args{'force'} = 1; # print even when errors
    pp_cbd(\%args,@cbd);
    pp_diagnostics(\%args) if pp_status();
} else {
    pp_diagnostics(\%args);
    # print Dumper \%map;
}

1;
