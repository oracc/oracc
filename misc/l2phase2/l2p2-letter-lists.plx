#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use Data::Dumper;

use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;

my $CBD = 'http://oracc.org/ns/cbd/1.0';
my $DC  = 'http://dublincore.org/documents/2003/06/02/dces/';

my $lang = shift @ARGV;

my $dirname = '01bld/'.$lang.'/';
my %letter_ids = ();
my @order = ();

my $x = load_xml("01bld/$lang/$lang.xml");

foreach my $l (tags($x,$CBD,'letter')) {
    #    warn $l->getAttributeNS($DC, 'title'), "\n";
    my $letter = $l->getAttributeNS($DC, 'title');
    push @order, $letter;
    foreach my $e (tags($l,$CBD,'entry')) {
#	warn xid($e), "\n";
	push @{$letter_ids{$letter}}, xid($e);
    }
}

my $lid = 'L000';
my @l = ();
foreach my $l (@order) {
    open(L,">$dirname$lid.lst");
    foreach my $id (@{$letter_ids{$l}}) {
	print L "$id\n";
    }
    close(L);
    push @l, [ $l, $lid++ ];
}
open(L,">${dirname}letter_ids.tab");
foreach my $l (@l) {
    print L "$$l[0]\t$$l[1]\t";
}
close(L);

1;
