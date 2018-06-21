#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use Data::Dumper;

use ORACC::CBD::XML;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::CBD::Senses;

my %args = pp_args();
my %base_senses = ();
my @base_cbd = ();
my $curr_entry = '';

if ($args{'base'}) {
    @base_cbd = setup_cbd(\%args,$args{'base'});
    for (my $i = 0; $i <= $#base_cbd; ++$i) {
	if ($base_cbd[$i] =~ /^\@entry\S*\s+(.*?)\s*$/) {
	    $curr_entry = $1;
	    my $cf = $curr_entry;
	    $cf =~ s/\s+\[.*$//;
#	    if ($cf =~ /\s/) {
#		$base_cpd_flags{$curr_entry} = 1;
#	    }
#	} elsif ($base_cbd[$i] =~ /^\@bases/) {
#	    $base_bases{$curr_entry} = $i;
	}
    }
    %base_senses = senses_collect(@base_cbd);
}

my @cbd = setup_cbd(\%args);

# print Dumper \%base_senses;

my @senses = ();
for (my $i = 0; $i <= $#cbd; ++$i) {
    if ($cbd[$i] =~ /^\@entry\S*\s+(.*?)\s*$/) {
	$curr_entry = $1;
    } elsif ($cbd[$i] =~ /^\@sense/) {
	push @senses, $cbd[$i];
    } elsif ($cbd[$i] =~ /^\@end\s+entry/) {
	if ($#senses >= 0) {
	    my $senses_b = $base_senses{$curr_entry};
	    if ($senses_b) {
		my $senses_b_str = senses_string($senses_b);
		my $senses_i_str = senses_string([ @senses ]);
		warn "aligning:\n\t$senses_i_str\ninto\t$senses_b_str\n";
		my $nsenses = senses_merge([ @senses ], $senses_b);
		$senses_b_str = senses_string($nsenses);
		warn "=>\t$senses_b_str\n";
	    }
	    @senses = ();
	} else {
	    warn "$0: no \@senses in $curr_entry\n";
	}
    }    
}

pp_diagnostics() if pp_status();

1;
