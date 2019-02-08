#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
use Data::Dumper;
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;

my $SLNS='http://oracc.org/ns/sl/1.0';

my $slxml = shift @ARGV;
$slxml = "02xml/ogsl-sl.xml" unless $slxml;

my $xsl = load_xml($slxml) || die;

my %groups = ();
foreach my $c ($xsl->getDocumentElement()->childNodes()) {
    my $n = $c->getAttribute('n');
    my ($first) = ($n =~ /([A-ZŠṢṬŊ0-9])/);
    if ($first) {
	push @{$groups{$first}}, $c;
    } else {
	push @{$groups{'?'}}, $c;
    }
}

open(EM,'>02xml/sl-grouped.xml'); select EM;

print '<signlist xmlns="http://oracc.org/ns/sl/1.0" xmlns:sl="http://oracc.org/ns/sl/1.0" xmlns:g="http://oracc.org/ns/gdl/1.0">';
foreach my $g (sort { &kcmp; } keys %groups) {
    my $lxid = 'l'.sprintf("%04d", ord($g));
    print "<letter name=\"$g\" title=\"$g\" xml:id=\"$lxid\">";
    my %sgroups = ();
    foreach my $s (@{$groups{$g}}) {
	my @snodes = tags($s,'http://oracc.org/ns/gdl/1.0','s');
	if ($#snodes >= 0) {
	    push @{$sgroups{$snodes[0]->textContent()}}, $s;
	} else {
	    my $xid = xid($s);
	    warn "sl-esp.plx: no s-node in $xid\n";
	}
    }
    foreach my $sc (sort keys %sgroups) {
	print "<signs name=\"$sc\" title=\"$sc\">";
	print map { $_->toString(0) } sort { &slsort } @{$sgroups{$sc}};
	print '</signs>';
    }
    print '</letter>';
}
print '</signlist>';

close(EM);

########################################################################################################

sub
slsort {
    my @asort = tags($a,$SLNS,'sort');
    my @bsort = tags($a,$SLNS,'sort');
    if ($#asort >= 0 && $#bsort >= 0) {
	$asort[0]->getAttribute('ogsl') <=> $bsort[0]->getAttribute('ogsl');
    } else {
	$#bsort <=> $#asort;
    }
}

sub
kcmp {
    my($akey,$bkey) = ($a,$b);
    $akey =~ tr/ABCDEFGHIJKLMNOPQRSTUVWXYZŊŠṢṬ.ₓ@%0-9₀-₉//cd;
    $bkey =~ tr/ABCDEFGHIJKLMNOPQRSTUVWXYZŊŠṢṬ.ₓ@%0-9₀-₉//cd;

    $akey =~ tr/ABCDEFGŊHIJKLMNOPQRSŠṢTṬUVWXYZ0123456789/ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmn/;
    $bkey =~ tr/ABCDEFGŊHIJKLMNOPQRSŠṢTṬUVWXYZ0123456789/ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmn/;
    
    my ($anum,$alet) = ($akey =~ /^(\d*)\(?(\S+)/);
    my ($bnum,$blet) = ($bkey =~ /^(\d*)\(?(\S+)/);
    my $res = 0;
    if ($anum && $bnum) {
	$res = $anum <=> $bnum;
	return $res unless $res == 0;
    } elsif ($anum) {
	return 1;
    } elsif ($bnum) {
	return -1;
    }
    return $alet cmp $blet;
}

1;
