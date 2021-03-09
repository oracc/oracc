#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

## This is the current fix approach, companion to texts/cbd-lem-fix.plx

use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::CBD::History;

use Data::Dumper;

my %args = pp_args(); $args{'mode'} = 'glossary';
$ORACC::CBD::nonormify = 1;

my @cbd = setup_cbd(\%args);

if (pp_status()) {
    pp_diagnostics();
    warn "$0: glossary must be clean before attempting to fix. Stop.\n";
    exit 1;
}

# get a hash of the changes made in the history file
my %h = ();
if ($args{'all'}) {
    %h = history_all_init(); # print Dumper \%h;
} else {
    %h = history_map();
}

$args{'output'} = 'trusted.glo';
open(L,'>trusted.log') || die "$0: can't open 'trusted.log'. Stop.\n";
my $curr_entry = '';
for (my $i = 0; $i <= $#cbd; ++$i) {
    if ($cbd[$i] =~ /^\@entry(\S*)\s+(.*?)\s*$/) {
	my ($x,$cfgw) = ($1,$2); $x = '' unless $x;
	$cfgw =~ s/\s+\[/[/; $cfgw =~ s/\]\s+/]/;
	$curr_entry = $cfgw;
	my $n = history_guess($cfgw);
	print L "trust entry $cfgw => $n\n";
	$cbd[$i] .= "\n>$n";
    } elsif ($cbd[$i] =~ /^\@sense(\S*)\s+(\S+)\s+(.*?)\s*$/) {
	my($x,$pos,$sns) = ($1,$2,$3); $x = '' unless $x;
	my $sig = $curr_entry;
	$sig =~ s#\]#//$sns]#;
	$sig .= "'$pos";
	my $n = history_guess_sense($sig);
	if ($n ne $sig) {
	    my($nsns,$npos) = ($n =~ m#//(.*?)\].*'(\S+)#);
	    my $orig = $cbd[$i];
	    print L "trust sense $orig => $npos $nsns\n";
	    $cbd[$i] .= "\n>$npos $nsns";
	}
    }
}
close(L);

pp_cbd(@cbd);

1;
