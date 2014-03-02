#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use ORACC::SL::BaseC;
my %signs = ();
my @signs = ();

while (<>) {
    chomp;
    my($sign, $count, $freq, ,@values) = split(/\t/, $_);
    push @signs, $sign;
    $signs{$sign} = [ $sign, $count, $freq , join("\t", @values) ];
}

ORACC::SL::BaseC::init();

print '<?xml version="1.0" encoding="utf-8"?>', "\n";
print '<repertoire xmlns="http://oracc.org/ns/sl/1.0">';
foreach my $s (@signs) {
    my $s0 = xmlify(${$signs{$s}}[0]);
    my $s1 = xmlify(${$signs{$s}}[1]);
    my $s2 = xmlify(${$signs{$s}}[2]);
    my($hex,$n) = ($s0 =~ /^(.*?):(.*?)$/);
    my $kn = $n;
    $kn =~ tr/|//d;
    $kn =~ s/\&amp;/&/g;
    my $id = ORACC::SL::BaseC::xid($kn) || '';
    warn "$kn not found in ogsl\n" unless $id;
    my $xn = xmlify($n);
    my $utf8 = $hex;
    $utf8 =~ s/\./;&#/g;
    $utf8 = "\&#$utf8;";
    print "<sign ref=\"$id\" n=\"$n\" hex=\"$hex\" icount=\"$s1\" ishare=\"$s2\" utf8=\"$utf8\">";
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
