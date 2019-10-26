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

set_default_base(\%args) unless $args{'base'};
$args{'log'} = 'align-senses.log' unless $args{'log'};
unlink $args{'log'};

if ($args{'base'}) {
    @base_cbd = setup_cbd(\%args,$args{'base'});
    if (pp_status()) {
	pp_diagnostics(\%args);
	die "$0: can't align bases unless base glossary is clean. Stop.\n";
    }
    $args{'lang'} = lang() unless $args{'lang'};
    $args{'output'} = "$args{'lang'}-senses-aligned.glo" unless $args{'output'};
} else {
    die "$0: must give base glossary with -base GLOSSARY\n";
}

my @cbd = setup_cbd(\%args);

senses_init(\%args);
my %map = senses_align(\%args, \@base_cbd, \@cbd);
senses_term();

open(M,'>senses-map.dump'); print M Dumper \%map; close(M);

my $curr_entry = '';

if ($args{'apply'}) {
    my $mapto = '';
    for (my $i = 0; $i <= $#cbd; ++$i) {
#	my $noprint_plus_1 = 0;
	if ($cbd[$i] =~ /^$acd_rx?\@entry\S*\s+(.*?)$/) {
	    $curr_entry = $1;
	    $curr_entry =~ s/\s+(\[.*?\])\s+/$1/;
	} elsif ($cbd[$i] =~ /^\@sense/) {
	    if ($map{$curr_entry}) {
		if (${$map{$curr_entry}}{$cbd[$i]}) {
		    $mapto = ${$map{$curr_entry}}{$cbd[$i]};
		    # warn "mapping $cbd[$i] => $mapto\n";
		    if ($cbd[$i+1] =~ /^>/) {
			warn "$cbd[$i+1]\n";
			my ($s) = ($cbd[$i+1] =~ /^>\s*(.*?)\s*$/);
			$s = "\@sense $s";
			if ($s ne $mapto) {
			    pp_line($i);
			    pp_warn("map-to sense in > line differs from stored sense ($s ne $mapto)");
			    $mapto = undef;
			} else {
			    # $noprint_plus_1 = 1;
			}
		    } else {
			$mapto =~ s/\@sense\s+// if $mapto; # new style drops @entry/@sense after >
			$cbd[$i] .= "\n>$mapto";
		    }
		}
	    }
	}
    }
    $args{'force'} = 1; # print even when errors
    pp_cbd(\%args,@cbd);
    pp_diagnostics(\%args) if pp_status();
} else {
    pp_diagnostics(\%args);
    system "grep '\[[34]\]' senses.log >senses-34.log";
}

1;
