#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use ORACC::L2GLO::Builtins;

my $glolang = shift @ARGV;
my $dirname = "01bld/$glolang/";
my $glofile = "${dirname}articles.xml";
my $xglo = load_xml($glofile) || die "l2p2-letters.plx: unable to read XML glossary $glofile\n";

my %letter_ids = ();

foreach my $e (tags($xglo,'http://oracc.org/ns/cbd/1.0', 'entry')) {
    my $entry = $e->getAttribute('n');
    my ($letter) = ($entry =~ /^(.)/);
    push @{$letter_ids{&ORACC::L2GLO::Builtins::first_letter($letter)}}, xid($e);
}

my $lid = 'L000';
my @l = ();
foreach my $l (sort keys %letter_ids) {
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
