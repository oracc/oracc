#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDOUT, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use Getopt::Long;

my $input = '';
my $oodns = 'http://oracc.org/ns/ood/1.0';
my $strip = 'Division,Inscribed,Comments,Unpublished';

GetOptions(
    'i:s'=>\$input,
    's:s'=>\$strip,
    );

die "ood-strip.plx: must name input with -i arg\n" unless $input;
die "ood-strip.plx: must tell me which fields to strip with -s arg\n" unless $strip;

my %strip = ();
@strip{split(/,/,$strip)} = ();

my $x = load_xml($input) || die "ood-strip.plx: failed to open XML input '$input'\n";

foreach my $r ($x->getElementsByTagNameNS($oodns,'record')) {
    my @deletiae = ();
    foreach my $c ($r->childNodes()) {
	if (exists $strip{$c->nodeName()}) {
	    push @deletiae, $c;
	}
    }
    foreach my $c (@deletiae) {
	$r->removeChild($c);
    }
}

print $x->toString();

1;
