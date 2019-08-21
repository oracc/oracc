#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use Data::Dumper;
use lib "$ENV{'ORACC'}/lib";
use Getopt::Long;
use ORACC::CBD::Util;
use ORACC::CBD::C11e;
use ORACC::CBD::Validate;

my $acd_rx = $ORACC::CBD::acd_rx;
my %args = pp_args();
my @cbd = setup_cbd(\%args);
my $lang = lang();
my $cfgwpos = '';

for (my $i = 0; $i <= $#cbd; ++$i) {
    if ($cbd[$i] =~ /^$acd_rx\@entry\S*\s+(.*?)\s*$/) {
	$cfgwpos = $1;
	$cfgwpos =~ s/\s+\[/[/;
	$cfgwpos =~ s/\]\s+/]/;
	print "$lang\t$cfgwpos\tword\n";
    } elsif ($cbd[$i] =~ /^$acd_rx\@sense\S*\s+(\S+)\s+(.*?)\s*$/) {
	my($epos,$sense) = ($1,$2);
	$sense =~ s/\s+\*\S+$//;
	my $ssig = $cfgwpos;
	$ssig =~ s#\]#//$sense]#;
	$ssig .= "'$epos";
	print "$lang\t$ssig\tsense\t$cfgwpos\n";
    }
}

1;

