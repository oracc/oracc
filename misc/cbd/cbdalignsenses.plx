#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use Data::Dumper;

use ORACC::CBD::XML;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::CBD::Senses;

my $acd_rx = $ORACC::CBD::acd_rx;

my %args = pp_args();
$ORACC::CBD::nonormify = 1;

my @base_cbd = ();
if ($args{'base'}) {
    @base_cbd = setup_cbd(\%args,$args{'base'});
} else {
    die "$0: must give base glossary with -base GLOSSARY\n";
}

my @cbd = setup_cbd(\%args);

senses_init(\%args);
my %map = senses_align(\%args, \@base_cbd, \@cbd);
senses_term();

open(M,'>map.dump'); print M Dumper \%map; close(M);

my $curr_entry = '';

if ($args{'apply'}) {
    my $mapto = '';
    for (my $i = 0; $i <= $#cbd; ++$i) {
	my $noprint_plus_1 = 0;
	if ($cbd[$i] =~ /^$acd_rx?\@entry\S*\s+(.*?)$/) {
	    $curr_entry = $1;
	} elsif ($cbd[$i] =~ /^\@sense/) {
	    if (${$map{$curr_entry}}{$cbd[$i]}) {
		$mapto = ${$map{$curr_entry}}{$cbd[$i]};
		warn "mapping $cbd[$i] => $mapto\n";
		if ($cbd[$i+1] =~ /^>/) {
		    my ($s) = ($cbd[$i+1] =~ /^>\s*(.*?)\s*$/);
		    $s = "\@sense $s";
		    if ($s ne $mapto) {
			warn "$.: $s ne $mapto\n";
			$mapto = undef;
		    } else {
			$noprint_plus_1 = 1;
		    }
		}
	    }
	}
	print $cbd[$i], "\n" unless $cbd[$i] =~ /^\000/;
	if ($mapto) {
	    print ">$mapto\n"; # CARE with this if we start handling '=' as well as '>'
	    $mapto = '';
	    ++$i if $noprint_plus_1;
	    $noprint_plus_1 = 0;
	}
    }
} else {
    pp_diagnostics() if pp_status();
}

1;
