#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use POSIX qw/strftime/;
use Data::Dumper;

use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;

my %d = ();

my $x = load_xml('public-projects.xml');
my @p = tags($x,undef,'project');
foreach my $p (@p) {
    my $n = $p->getAttribute('n');
    if (-r "$ENV{'ORACC_BUILDS'}/$n/installstamp") {
	my $t = (stat _)[9];
	my $s = strftime("%FT%TZ", gmtime($t));
	#warn "$n/installstamp t=$t s=$s\n";
	$p->setAttribute('date',$s);
	push @{$d{$s}}, $p;
    }
}
my $parent = $x->getDocumentElement();
$parent->removeChildNodes();

open(IX,'>public-project-dates.tab');
foreach my $d (sort { $a cmp $b } keys %d) {
    foreach my $n (@{$d{$d}}) {
	$parent->appendChild($n);
	print IX $n->getAttribute('n'), "\t$d\n";
    }
}
close(IX);

open(X,'>public-projects-ds.xml');
print X $x->toString();
close(X);

1;
