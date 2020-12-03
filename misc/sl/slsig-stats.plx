#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC_BUILDS'}/lib";
use Data::Dumper;
use ORACC::SL::BaseC;
use ORACC::L2GLO::Util;

# Read project.sig and generate FORM and BASE statistics by CFGWPOS and SL signatures

die "$0: no 01bld/project.sig\n" unless -r '01bld/project.sig';

my %index = ();

my @input = `cut -f1,3 01bld/project.sig`;
my %seen = ();

ORACC::SL::BaseC::init();
foreach (@input) {
    next if /^\{/ || /^\@fields/;
    chomp;
    my ($sig,$count) = split(/\t/,$_);
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
	    $s .= "$v<${$index{$_}}{$v}> " unless $seen{$v};
	}
	$s =~ s/\s$/\n/;
	push @bytlit, $s;
    } else {
	my $s = "$_\t";
	my %seen = ();
	foreach my $v (sort { ${$index{$_}}{$b} <=> ${$index{$_}}{$a} } keys %{$index{$_}}) {
	    $s .= "$v<${$index{$_}}{$v}> " unless $seen{$v};
	}
	$s =~ s/\s$/\n/;
	push @bycf, $s;
    }
}

print @bytlit;
print @bycf;

# print Dumper \%index;

1;
