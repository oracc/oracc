#!/usr/bin/perl
use warnings; use strict; use open 'utf8';

binmode STDOUT, 'utf8';

use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::XML;

my %l = ();
open(L,'labels.tab');
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
    my $rrl = $rr->getAttribute('name');
    my $rrl_rx = $rrl;
    $rrl_rx =~ s/^.*?_//;
    foreach my $r ($rr->childNodes()) {
	my $v = $r->textContent();
	$v =~ s/\+.*$//;
	$v =~ s/^.*?://;
	$v =~ s/\.[^.]+$//;
	if ($l{$v}) {
	    my $rl = $l{$v};
	    $r->setAttribute('label1',$l{$v});
	    $rl =~ s/^$rrl_rx\.//;
	    $r->setAttribute('label2',"$rrl.$rl");
	} else {
	    print "$v\n";
	}
    }
}

$f =~ s/\./+./;
open(X,">$f");
print X $x->toString();
close(X);

1;
