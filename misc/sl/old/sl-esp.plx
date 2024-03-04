#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
use Data::Dumper;
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
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

open(EM,'>02xml/sl-esp.xml'); select EM;

print '<em:site xmlns:em="http://oracc.org/ns/em/1.0" xmlns:g="http://oracc.org/ns/gdl/1.0">';
foreach my $g (sort keys %groups) {
    print "<em:group name=\"$g\" title=\"$g\">";
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
	print "<em:group name=\"$sc\" title=\"$sc\">";
	print map { $_->toString(0) } @{$sgroups{$sc}};
	print '</em:group>';
    }
    print '</em:group>';
}
print '</em:site>';

close(EM);

1;
