#!/usr/bin/perl
use warnings; use strict; use open 'utf8';

binmode STDOUT, 'utf8';

use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::XML;

my $dsc = '../../00lib/designation-sort-order.lst';
my %d = ();
if (-r $dsc) {
    my $i = 0;
    warn "reading $dsc ...\n";
    open(D, $dsc) || die "$0: can't open $dsc for read\n";
    while (<D>) {
	chomp;
	s/\s.*$//;
	$d{$_} = ++$i;
    }
    close(D);
}

#use Data::Dumper; warn Dumper \%d;

my %l = ();
open(L,'../labels.tab');
while (<L>) {
    chomp;
    my($id,$lab) = split(/\t/,$_);
    $l{$id} = $lab;
    $id =~ s/l$//;
    $l{$id} = $lab;
}
close(L);

my $f = shift @ARGV;
my $x = load_xml($f);

my @rr = tags($x,'http://oracc.org/ns/xis/1.0','rr');
foreach my $rr (@rr) {
    my $val = $rr->getAttribute('value');
    my ($pqx) = ($val =~ /([PQX]\d\d\d\d\d\d)/);
    warn "$0: $pqx not in $dsc\n" unless $d{$pqx};
    my $c = $d{$pqx} || 100000;
    $rr->setAttribute('c',$c);
    my $rrl = $rr->getAttribute('name');
    my $rrl_rx = $rrl;
    $rrl_rx =~ s/^.*?_//;
    my $last_pre_colon = '';
    foreach my $r ($rr->childNodes()) {
	my $v = $r->textContent();
	$v =~ s/\+.*$//;
	$v =~ s/^.*?://;
	$v =~ s/\.[^.]+$//;
	my $c = $v;
	$c =~ s/^.*?\.//;
	$r->setAttribute('c',$c);
	my $same_text = 0;
	if ($l{$v}) {
	    my $rl = $l{$v};
	    $r->setAttribute('label1',$l{$v});
	    $rl =~ s/^$rrl_rx[.:]//;
	    my $punct = ($rl =~ /:/ ? '.' : ':');
	    $r->setAttribute('label2', "$rrl$punct$rl");
	} else {
	    warn "$0: $v not in labelset\n";
	}
    }
}

$f =~ s/\./+./;
open(X,">$f");
print X $x->toString();
close(X);

1;
