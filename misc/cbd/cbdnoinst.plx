#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use Data::Dumper;
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::Util;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Hash;

my %args = pp_args();

my @cbd = setup_cbd(\%args);

if (pp_status()) {
    my $ret = pp_diagnostics(\%args);
    die("cbdnoinst.plx: $ret errors in glossary $args{'cbd'}. Stop.\n");
}

my $h = pp_hash(\%args,@cbd);

my $summaries = '01bld/'.lang().'/summaries.xml'; 
my @counts = `xsltproc $ENV{'ORACC_BUILDS'}/lib/scripts/entry-counts.xsl $summaries`;
chomp @counts;

my %counts = ();
foreach my $c (@counts) {
    my($e,$n) = split(/\t/,$c);
    $e =~ s/\[(.*?)\]/ [$1] /;
    $counts{$e} = $n;
}

my @cfgws = pp_hash_cfgws($h);
if ($args{'edit'}) {
    for (my $i = 0; $i <= $#cbd; ++$i) {
	if ($cbd[$i] =~ /\@entry\s+(.*?)\s*$/) {
	    my $cfgw = $1;
	    unless ($counts{$cfgw}) {
		warn "removing $cfgw because no instances\n";
		do {
		    $cbd[$i++] = "\000";
		} until ($cbd[$i] =~ /\@end\s+entry/);
		$cbd[$i] = "\000";
	    }
	}
    }
    pp_trace("cbdpp/writing cbd");
    pp_cbd(\%args,@cbd) if $args{'edit'};
    pp_trace("cbdpp/cbd write complete");
} else {
    foreach my $c (@cfgws) {
	next if $counts{$c};
	warn "$c has no instances\n";
    }
}

pp_diagnostics(\%args);

1;
