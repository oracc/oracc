#!/usr/bin/perl
use warnings; use strict;
use open 'utf8'; binmode STDERR, 'utf8'; binmode STDIN, 'utf8'; binmode STDOUT, 'utf8';

use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::XML;

my %map = (
    CN=>'DN',
    DN=>'DN',
    EN=>'GN',
    GN=>'GN',
    MN=>'MN',
    ON=>'TN',
    PN=>'PN',
    QN=>'GN',
    RN=>'PN',
    SN=>'GN',
    TN=>'TN',
    WN=>'GN',   
    );
my %bit = ();

my $f = shift @ARGV;
my $x = load_xml($f);

my @entries = tags($x, 'http://oracc.org/ns/cbd/1.0', 'entry');

foreach my $e (@entries) {
    my $n = ($e->childNodes())[2];
    my $pos = $n->textContent();
    warn "POS $pos not in map\n" and next unless $map{$pos};
    push @{$bit{$map{$pos}}}, $e;
}

system 'rm', '-fr', 'qpn-bits';
system 'mkdir', '-p', 'qpn-bits';
foreach $b (keys %bit) {
    my $o = "qpn-bits/qpn-x-$b.xml";
    open(O,">$o") || die;
    print O '<entries xmlns="http://oracc.org/ns/cbd/1.0" xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"';
    print O " title=\"$b\">";
    foreach my $e (@{$bit{$b}}) {
	print O $e->toString();
    }
    print O '</entries>';
    close(O);
}

1;
