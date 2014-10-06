#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
use Getopt::Long;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use ORACC::SL::BaseC;

my $numeric = 0;
my %signs = ();
my @signs = ();

GetOptions (
    "num:i"=>\$numeric,
    );

while (<>) {
    chomp;
    my($sign, $count, $freq, ,@values) = split(/\t/, $_);
    if ($numeric) {
	foreach my $v (@values) {
	    my $val = $v;
	    $v =~ s/\s+.*$//;
	    my $xsign = "$sign:\U$v";
	    push @signs, $xsign;
	    $signs{$xsign} = [ $xsign, $count, $freq , $val ];
	}
    } else {
	push @signs, $sign;
	$signs{$sign} = [ $sign, $count, $freq , join("\t", @values) ];
    }
}

ORACC::SL::BaseC::init();

print '<?xml version="1.0" encoding="utf-8"?>', "\n";
print '<repertoire xmlns="http://oracc.org/ns/sl/1.0">';
foreach my $s (@signs) {
    my $s0 = xmlify(${$signs{$s}}[0]);
    my $s1 = xmlify(${$signs{$s}}[1]);
    my $s2 = xmlify(${$signs{$s}}[2]);
    my($hex,$n,$nn) = split(/:/, $s0); # ($s0 =~ /^(.*?):(.*?)$/);
    $nn = $n unless $nn;
    my $kn = $n;
    $kn =~ tr/|//d;
    $kn =~ s/\&amp;/&/g;
    my $id = ORACC::SL::BaseC::xid($kn) || '';
    warn "$kn not found in ogsl\n" unless $id;
    my $xn = xmlify($nn);
    my $utf8 = $hex;
    warn "$kn = utf8 = $utf8\n";
    if ($utf8 =~ /^(?:x[a-f0-9]{5}\.?)+$/i) {
	$utf8 =~ s/\./;&#/g;
	$utf8 = "\&#$utf8;";
    } else {
	$utf8 = '';
    }
    my $countunit = '';
    if ($numeric) {
	my($count,$unit) = ($xn =~ /^(.*?)\((.*?)\)$/);
	warn "bad num $xn\n" unless $count && $unit;
	$countunit = " numc=\"$count\" numu=\"$unit\"";
    }
    print "<sign ref=\"$id\" n=\"$xn\" hex=\"$hex\" icount=\"$s1\" ishare=\"$s2\" utf8=\"$utf8\"$countunit>";
    my @vals = split(/\t/, ${$signs{$s}}[3]);
    print '<vals>';
    foreach my $v (@vals) {
	my($vv,$ct,$pc) = ($v =~ m,^(\S+)\s+(\d+)/(\d+)$,);
	if ($vv) {
	    my $xv = xmlify($vv);
	    print "<v icount=\"$ct\" ipct=\"$pc\">$xv</v>";
	}
    }
    print '</vals>';
    print '</sign>';
}
print '</repertoire>';

ORACC::SL::BaseC::term();

1;
