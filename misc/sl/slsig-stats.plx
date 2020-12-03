#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC_BUILDS'}/lib";
use Data::Dumper;
use ORACC::SL::BaseC;
use ORACC::L2GLO::Util;
use Getopt::Long;

my $force = 0;
my $project = '';

GetOptions(
    f=>\$force,
    'p:s'=>\$project,
    );

# Read project.sig and generate FORM and BASE statistics by CFGWPOS and SL signatures

my $sigs = '01bld/project.sig'; $sigs = "$ENV{'ORACC_BUILDS'}/$project/$sigs" if $project;

die "$0: no $sigs\n" unless -r $sigs;

my $stats = $sigs; $stats =~ s/\.sig$/.stats/ || die "$0: sigs file must end in .sig\n";

my $sigs_date = (stat($sigs))[9];
my $stats_date = (stat($stats))[9];

if (defined($stats_date) && ($sigs_date < $stats_date)) {
    if ($force) {
	warn "$0: $sigs older than $stats; forcing rebuild\n";
    } else {
	warn "$0: $sigs older than $stats; no need to rebuild\n";
	exit 0;
    }
} else {
    warn "$0: no $stats file, building from $sigs\n";
}

my $union = "01bld/sux/union.sig"; $union = "$ENV{'ORACC_BUILDS'}/$project/$union" if $project;

open(STATS, ">$stats") || die "$0: unable to write stats file $stats\n"; select STATS;

my %forms = ();
my %index = ();

my @input = `cut -f1,3 $sigs`;
my @more = `cut -f1 $union`;
push @input, @more;
my %seen = ();
my %seensig = ();

ORACC::SL::BaseC::init();
foreach (@input) {
    next if /^\{/ || /^\@(?:fields|project|lang|name)/ || /^\s*$/;
    chomp;
    my ($sig,$count) = split(/\t/,$_); $count = 0 unless defined $count;
    next if $seensig{$sig}++;
    my %s = parse_sig($sig);
    my $cfgwpos = "$s{'cf'}\[$s{'gw'}\]$s{'pos'}";
    my $base = $s{'base'}; warn "$0: bad base in $_\n" unless $base;
    my $form = $s{'form'}; warn "$0: bad form in $_\n" unless $form;
    $base =~ tr/·°//d; $form =~ s/\\.*$//;
    $base =~ s/^\%.*?://; $form =~ s/^\%.*?://;
    my $bs = ORACC::SL::BaseC::tlit_sig('',$base);
    my $fs = ORACC::SL::BaseC::tlit_sig('',$form);
    unless ($bs =~ /q/) {
	${$index{$bs}}{"$cfgwpos"} += $count; # most frequent lemma for base sig
	${$index{"$cfgwpos\::/$bs"}}{$base} += $count unless $bs =~ /q/; # most frequent tlit of base in lemma
    }
    unless ($fs =~ /q/) {
	${$index{$fs}}{"$cfgwpos"} += $count; # most frequent lemma for form sig
	${$index{"$cfgwpos\::=$fs"}}{$form} += $count unless $fs =~ /q/; # most frequent tlit of form in lemma
	my $f = "/$s{'base'} "; $f =~ s#\%.*?:##;
	$f .= "+$s{'cont'} " if $s{'cont'};
	$f .= "#$s{'morph'}";
	$forms{"$cfgwpos\::=$fs"} = $f;
    }
}
ORACC::SL::BaseC::term();

my @bycf = ();
my @bytlit = ();

foreach (sort keys %index) {
    if (/::/) {
	my($cf,$type,$sig) = (m#^(.*?)::([/=])(.*?)$#);
	my $s = "$sig\::$cf\t$type";
	foreach my $v (sort { ${$index{$_}}{$b} <=> ${$index{$_}}{$a} } keys %{$index{$_}}) {
	    $s .= "$v<${$index{$_}}{$v}>\t" unless $seen{$v};
	}
	$s =~ s/\s$/\n/;
	push @bytlit, $s;	
    } else {
	my $s = "$_\t";
	my %seen = ();
	foreach my $v (sort { ${$index{$_}}{$b} <=> ${$index{$_}}{$a} } keys %{$index{$_}}) {
	    $s .= "$v<${$index{$_}}{$v}>\t" unless $seen{$v};
	}
	$s =~ s/\s$/\n/;
	push @bycf, $s;
    }
}

print @bytlit;
print @bycf;
foreach my $f (sort keys %forms) {
    print "$f\t$forms{$f}\n";
}

close STATS;

1;
