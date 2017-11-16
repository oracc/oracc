#!/usr/bin/perl
use warnings; use strict;

my %atf = ();

open(O, '00lib/order.lst') 
    || die "neo-proxies.plx: can't open neo/00lib/order.lst\n";
while (<O>) {
    chomp;
    my $prj = $_;
    my $atf = "../$prj/01bld/lists/have-lem.lst";
    if (-r $atf) {
	if (open(A,$atf)) {
	    while (<A>) {
		my($p,$P,$c) = ();
		if (/\@/) {
		    ($p,$P,$c) = (/^(.*?):(.*?)\@(.*)$/);
		} else {
		    ($p,$P) = (/^(.*?):(.*?)$/);
		    $c = '';
		}
		if ($p eq $prj && $P =~ /^[PQ]/) {
		    push @{$atf{$P}}, $p;
		} # ignore proxies and X-numbers
	    }
	    close(A);
	} else {
	    warn "neo-proxies.plx: can't open $atf, skipping it.\n";
	}
    }
}
close(O);

my $i = 0;
my $l = 0;
open(P, ">00lib/neo.lst");
foreach my $pq (sort keys %atf) {
    ++$l;
    foreach my $p (@{$atf{$pq}}) {
	++$i;
	print P "$p\:$pq ";
	
    }
    print P "\n";
}
close(P);

warn "NEO: found $i ATF editions of $l P/Q numbers\n";

1;
