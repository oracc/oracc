#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use POSIX qw/strftime/;
use Data::Dumper;

use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;

my $x = load_xml('public-projects.xml');
my @p = tags($x,undef,'project');
foreach my $p (@p) {
    my $n = $p->getAttribute('n');
    if (-r "$ENV{'ORACC_BUILDS'}/$n/installstamp") {
	my $t = (stat _)[9];
	my $s = strftime("%FT%TZ", gmtime($t));
	#warn "$n/installstamp t=$t s=$s\n";
	$p->setAttribute('date',$s);
    }
}

open(X,'>public-projects-ds.xml');
print X $x->toString();
close(X);

1;
