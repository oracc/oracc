#!/usr/bin/perl
use warnings; use strict;

my %atf = ();

open(O, '00lib/order.lst') 
    || die "neo-proxies.plx: can't open neo/00lib/order.lst\n";
while (<O>) {
    chomp;
    my $prj = $_;
    my $atf = "../$prj/01bld/lists/have-atf.lst";
    open(A,$atf) || die "neo-proxies.plx: can't open $atf\n";
    while (<A>) {
	my($p,$P,$c) = ();
	if (/\@/) {
	    ($p,$P,$c) = (/^(.*?):(.*?)\@(.*)$/);
	} else {
	    ($p,$P) = (/^(.*?):(.*?)\@(.*)$/);
	    $c = '';
	}
	if ($p eq $prj && $P =~ /^[PQ]/) {
	    push @{$atf{$P}}, $p;
	} # ignore proxies and X-numbers
    }
    close(A);
}
close(O);

open(P, ">00lib/neo.lst");
foreach my $pq (sort keys %atf) {
    foreach my $p (@{$atf{$pq}}) {
	print P "$p\:$pq ";
    }
    print P "\n";
}
close(P);

1;
