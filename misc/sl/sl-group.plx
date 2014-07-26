#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
use Data::Dumper;
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;

my $SLNS='http://oracc.org/ns/sl/1.0';

my $xsl = load_xml("02xml/ogsl-sl.xml") || die;

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
foreach my $g (sort keys %groups) {
    print "<letters name=\"$g\" title=\"$g\">";
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
    print '</letters>';
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

1;
